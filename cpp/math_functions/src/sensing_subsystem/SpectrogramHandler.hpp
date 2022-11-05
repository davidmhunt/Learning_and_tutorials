#ifndef SPECTROGRAMHANDLER
#define SPECTROGRAMHANDLER

    //include header files
    #include "pocketfft/pocketfft_hdronly.h"

    //C standard libraries
    #include <iostream>
    #include <cstdlib>
    #include <string>
    #include <complex.h>
    #include <vector>
    #include <csignal>
    #include <thread>

    #define _USE_MATH_DEFINES
    #include <cmath>

    //including buffer handler
    #include "../BufferHandler.hpp"

    using namespace Buffers;
    using namespace pocketfft;

    namespace SpectrogramHandler_namespace {

        template<typename data_type>
        class SpectrogramHandler
        {
        private:

            //size parameters
            size_t samples_per_sampling_window;
            size_t fft_size;
            size_t num_rows;
            size_t num_samples_per_spectrogram;

            //fft parameters
            shape_t shape;
            stride_t stride;
            shape_t axes;

            //peak_detection_parameters
            data_type peak_detection_threshold;

        
        public:

            //parameters for ffts

                //reshaped and window buffer (ready for FFT)
                Buffer_2D<std::complex<data_type>> reshaped__and_windowed_signal_for_fft;

                //hanning window
                Buffer_1D<std::complex<data_type>> hanning_window;

                //computed FFT vector
                Buffer_2D<std::complex<data_type>> computed_fft;

                //generated spectrogram
                Buffer_2D<data_type> generated_spectrogram;

                //spectrogram points
                Buffer_1D<data_type> spectrogram_points_values;
                Buffer_2D<size_t> spectrogram_points_indicies;

            // detected peaks

            //detected times and frequencies
            
        public:

            /**
             * @brief Construct a new Spectrogram Handler object
             * 
             * @param num_rows the number of rows for the reshaped signal and the fft computation
             * @param samples_per_sampling_window  the number of samples in each sampling window
             * @param fft_size the size of the fft to compute for each sampling window
             */
            SpectrogramHandler(size_t desired_num_rows, 
                size_t desired_samples_per_sampling_window,
                size_t desired_fft_size)
                : num_rows(desired_num_rows),
                samples_per_sampling_window(desired_samples_per_sampling_window),
                fft_size(desired_fft_size),
                num_samples_per_spectrogram(num_rows * samples_per_sampling_window),
                shape({fft_size}),
                stride({sizeof(std::complex<data_type>)}),
                axes({0}),
                peak_detection_threshold(7),
                reshaped__and_windowed_signal_for_fft(num_rows,fft_size),
                hanning_window(fft_size),
                computed_fft(num_rows,fft_size),
                generated_spectrogram(num_rows,fft_size),
                spectrogram_points_values(num_rows),
                spectrogram_points_indicies(num_rows,2)
                {

                    // initialize the hanning window
                    initialize_hanning_window(fft_size);
                    
                return;
            }
            ~SpectrogramHandler() {};

            /**
             * @brief Computes a hanning window of a given size for use in the spectogram generation
             * 
             * @param fft_size the size of the fft's that will be computed (hanning window is same size)
             */
            void initialize_hanning_window(size_t fft_size) {
                data_type M = static_cast<data_type>(fft_size);
                for (size_t i = 0; i < fft_size; i++)
                {
                    data_type n = static_cast<data_type>(i);
                    //data_type x = 2 * M_PI * n / (M - 1);
                    //data_type cos_x = cos(x);

                    data_type hann = 0.5 * (1 - cos(2 * M_PI * n / (M - 1)));

                    hanning_window.buffer[i] = std::complex<data_type>(hann);
                } 
            }
        
            /**
             * @brief Loads a received signal, reshapes, and prepares it 
             * for fft processing. Signal is saved in 
             * reshaped__and_windowed_signal_for_fft buffer
             * 
             * @param received_signal a 2D vector of the received signal recorded on the USRP
             */


            /**
             * @brief Loads a received signal, reshapes, and prepares it 
             * for fft processing. Signal is saved in 
             * reshaped__and_windowed_signal_for_fft buffer
             * 
             * @param received_signal a 2D vector of the received signal recorded on the USRP
             */
            void load_and_prepare_for_fft(std::vector<std::vector<std::complex<data_type>>> & received_signal){
                
                //get dimmensions of received_signal array
                size_t m = received_signal.size(); //rows
                size_t n = received_signal[0].size(); //cols

                //initialize variables for reshaping
                size_t from_r;
                size_t from_c;

                //initialize variable to determine the coordinate in the received signal for a row/col index in reshpaed signal
                size_t k;

                for (size_t i = 0; i < num_rows; i++)
                {
                    for (size_t j = 0; j < fft_size; j++)
                    {
                        //for a given row,col index in the reshaped signal, determine the coordinate in the received_signal
                        size_t k = i * samples_per_sampling_window + j;

                        //indicies in received_signal
                        from_r = k/n;
                        from_c = k % n;

                        if (from_r >= m)
                        {
                            reshaped__and_windowed_signal_for_fft.buffer[i][j] = 0;
                        }
                        else{
                            reshaped__and_windowed_signal_for_fft.buffer[i][j] = received_signal[from_r][from_c] * hanning_window.buffer[j];
                        }
                    } 
                }
                return;
            }

            void compute_ffts(size_t start_idx = 0, size_t end_idx = 0){
              
              //if the end_idx is zero (default condition), set it to be the num_rows
              if (end_idx == 0)
              {
                end_idx = num_rows;
              }
              
              
              //compute the fft and generate the spectrogram for the given rows
              for (size_t i = start_idx; i < end_idx; i++)
                {
                    c2c(shape, stride, stride, axes, FORWARD,
                        reshaped__and_windowed_signal_for_fft.buffer[i].data(),
                        computed_fft.buffer[i].data(), (data_type) 1.);

                    //convert to dB
                    for (size_t j = 0; j < fft_size; j++)
                    {
                        generated_spectrogram.buffer[i][j] = 10 * std::log10(std::abs(
                            computed_fft.buffer[i][j]
                        ));
                    }  
                }
            }

            void compute_ffts_multi_threaded(size_t num_threads = 1){
                //initialize a vector of threads
                std::vector<std::thread> threads;
                size_t rows_per_thread = num_rows / num_threads;
                size_t start_row;
                size_t end_row;

                //spawn multiple threads
                for (size_t thread_num = 0; thread_num < num_threads; thread_num++)
                {   
                    //determine the start and end row for each thread
                    start_row = thread_num * rows_per_thread;

                    if (thread_num == (num_threads - 1)){
                        end_row = num_rows;
                    }
                    else{
                        end_row = (thread_num + 1) * rows_per_thread;
                    }

                    //spawn the thread
                    threads.push_back(std::thread([&] () {
                        compute_ffts(start_row,end_row);
                    }));
                }

                //join the threads
                for (auto& t : threads){
                    t.join();
                }
            }
        
            /**
             * @brief Compute the maximum value and its index in the given signal
             * 
             * @param signal the signal to determine the maximum value of
             * @return std::tuple<data_type,size_t> the maximum value and index of the maximum value in the signal
             */
            std::tuple<data_type,size_t> compute_max_val(std::vector<data_type> & signal){
                //set asside a variable for the value and index of the max value
                data_type max = signal[0];
                size_t idx = 0;

                for (size_t i = 0; i < signal.size(); i++)
                {
                    if (signal[i] > max)
                    {
                        max = signal[i];
                        idx = i;
                    }
                }
                return std::make_tuple(max,idx);
            }

            /**
             * @brief Detect the peaks in the computed spectrogram
             * 
             */
            void detect_peaks_in_spectrogram(){
                //initialize variable to store results from compute_max_val
                std::tuple<data_type,size_t> max_val_and_idx;
                data_type max_val;
                size_t idx;

                //variable to track the absolute maximum value detected in the spectrogram
                data_type absolute_max_val = generated_spectrogram.buffer[0][0];

                //get the maximum_value from each computed_spectrogram
                for (size_t i = 0; i < num_rows; i++)
                {
                    max_val_and_idx = compute_max_val(generated_spectrogram.buffer[i]);
                    max_val = std::get<0>(max_val_and_idx);
                    idx = std::get<1>(max_val_and_idx);
                    spectrogram_points_values.buffer[i] = max_val;
                    spectrogram_points_indicies.buffer[i][0] = idx;
                    spectrogram_points_indicies.buffer[i][1] = i;

                    //update the max value
                    if (max_val > absolute_max_val)
                    {
                        absolute_max_val = max_val;
                    }
                }
                
                data_type threshold = absolute_max_val - peak_detection_threshold;
                //go through the spectrogram_points and zero out the points below the threshold
                for (size_t i = 0; i < num_rows; i ++){
                    if (spectrogram_points_values.buffer[i] <= threshold)
                    {
                        spectrogram_points_values.buffer[i] = 0;
                        spectrogram_points_indicies.buffer[i][0] = 0;
                        spectrogram_points_indicies.buffer[i][1] = 0;
                    }
                }
                return;
            }
        };
    }


#endif
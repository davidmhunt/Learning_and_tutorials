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
        
        public:

            //parameters for ffts
                //reshaped buffer (not for fft)
                Buffer_2D<std::complex<data_type>> reshaped_signal;

                //reshaped and window buffer (ready for FFT)
                Buffer_2D<std::complex<data_type>> reshaped__and_windowed_signal_for_fft;

                //hanning window
                Buffer_1D<std::complex<data_type>> hanning_window;

                //computed FFT vector
                Buffer_2D<std::complex<data_type>> computed_fft;

                //generated spectrogram
                Buffer_2D<data_type> generated_spectrogram;

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
                reshaped_signal(num_rows,samples_per_sampling_window),
                reshaped__and_windowed_signal_for_fft(num_rows,fft_size),
                hanning_window(fft_size),
                computed_fft(num_rows,fft_size),
                generated_spectrogram(num_rows,fft_size)
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
            void load_and_prepare_for_fft(std::vector<std::vector<std::complex<data_type>>> & received_signal){
                
                //reshape the data into individual sampling windows
                reshaped_signal.load_data_into_buffer_efficient(received_signal,false);

                for (size_t i = 0; i < num_rows; i++)
                {
                    for (size_t j = 0; j < fft_size; j++)
                    {
                        reshaped__and_windowed_signal_for_fft.buffer[i][j] = 
                            reshaped_signal.buffer[i][j] * hanning_window.buffer[j];
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
        };
    }


#endif
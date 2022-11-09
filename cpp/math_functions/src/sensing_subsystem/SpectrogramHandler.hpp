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

    //include the JSON handling capability
    #include <nlohmann/json.hpp>

    using namespace Buffers;
    using namespace pocketfft;
    using json = nlohmann::json;

    namespace SpectrogramHandler_namespace {

        template<typename data_type>
        class SpectrogramHandler
        {
        private:

            json config;
            
            //size parameters
            size_t samples_per_sampling_window;
            size_t fft_size;
            size_t num_rows_rx_signal; //for the received signal
            size_t num_rows_spectrogram; //for the reshaped array (in preparation for spectogram)
            size_t num_samples_rx_signal;
            size_t num_samples_per_spectrogram; //for the reshaped spectrogram

            //fft parameters
            shape_t shape;
            stride_t stride;
            shape_t axes;

            //peak_detection_parameters
            data_type peak_detection_threshold;

            //frequency and timing variables
            data_type FMCW_sampling_rate;
            data_type frequency_resolution;
            data_type frequency_sampling_period;
            data_type detected_time_offset;
            std::vector<data_type> frequencies;
            std::vector<data_type> times;

            //clustering parameters
            size_t min_points_per_chirp;

        
        public:

            //buffers used

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
                Buffer_1D<size_t> spectrogram_points_indicies;

                //frequency and timing bins
                Buffer_1D<data_type> detected_times;
                Buffer_1D<data_type> detected_frequencies;

                //cluster indicies
                Buffer_1D<int> cluster_indicies;
            
        public:

            /**
             * @brief Construct a new Spectrogram Handler object using a config file
             * 
             * @param json_config a json object with configuration information 
             */
            SpectrogramHandler(json json_config): config(json_config){
                if (check_config())
                {
                    initialize_spectrogram_params();
                    initialize_fft_params();
                    initialize_buffers();
                    initialize_hanning_window();
                    initialize_freq_and_timing_bins();
                    initialize_clustering_params();
                }
                
                
            }

            ~SpectrogramHandler() {};

            /**
             * @brief Check the json config file to make sure all necessary parameters are included
             * 
             * @return true - JSON is all good and has required elements
             * @return false - JSON is missing certain fields
             */
            bool check_config(){
                bool config_good = true;
                //check sampling rate
                if(config["USRPSettings"]["Multi-USRP"]["sampling_rate"].is_null()){
                    std::cerr << "SpectrogramHandler::check_config: no sampling_rate in JSON" <<std::endl;
                    config_good = false;
                }
                
                //check the samples per buffer
                if(config["USRPSettings"]["RX"]["spb"].is_null()){
                    std::cerr << "SpectrogramHandler::check_config: Rx spb not specified" <<std::endl;
                    config_good = false;
                }

                //check the minimum recording time
                if(config["SensingSubsystemSettings"]["min_recording_time_ms"].is_null()){
                    std::cerr << "SpectrogramHandler::check_config: min recording time not specified" <<std::endl;
                    config_good = false;
                }

                if(config["SensingSubsystemSettings"]["spectogram_peak_detection_threshold_dB"].is_null()){
                    std::cerr << "SpectrogramHandler::check_config: spectogram peak detection threshold not specified" <<std::endl;
                    config_good = false;
                }

                if(config["SensingSubsystemSettings"]["min_points_per_chirp"].is_null()){
                    std::cerr << "SpectrogramHandler::check_config: min number of points per chirp not specified" <<std::endl;
                    config_good = false;
                }

                return config_good;
            }
            
            /**
             * @brief Initialize spectrogram parameters based on the configuration file
             * 
             */
            void initialize_spectrogram_params(){

                //specify the sampling rate
                FMCW_sampling_rate = config["USRPSettings"]["Multi-USRP"]["sampling_rate"].get<data_type>();
                size_t samples_per_buffer = config["USRPSettings"]["RX"]["spb"].get<size_t>();

                //determine the frequency sampling period based on the sampling rate
                data_type freq_sampling_period;
                if (FMCW_sampling_rate > 500e6)
                {
                    freq_sampling_period = 0.5e-6;
                }
                else{
                    freq_sampling_period = 2e-6;
                }
                
                //determine the number of samples per sampling window
                samples_per_sampling_window = static_cast<size_t>(std::ceil(FMCW_sampling_rate * freq_sampling_period));

                //determine the fft size
                fft_size = static_cast<size_t>(
                                std::pow(2,std::floor(
                                    std::log2(static_cast<data_type>(samples_per_sampling_window)))));

                //recompute the actual frequency sampling window using the number of samples 
                // per sampling window
                freq_sampling_period = static_cast<data_type>(samples_per_sampling_window) /
                                            FMCW_sampling_rate;
                
                //determine the number of rows in the rx signal buffer
                data_type row_period = static_cast<data_type>(samples_per_buffer)/FMCW_sampling_rate;
                data_type min_recording_time_ms = config["SensingSubsystemSettings"]["min_recording_time_ms"].get<data_type>();
                num_rows_rx_signal = static_cast<size_t>(std::ceil((min_recording_time_ms * 1e-3)/row_period));

                //determine the number of samples per rx signal
                num_samples_rx_signal = num_rows_rx_signal * samples_per_buffer;

                //determine the number of rows in the spectrogram
                num_rows_spectrogram = (num_samples_rx_signal / samples_per_sampling_window);
                num_samples_per_spectrogram = num_rows_spectrogram * samples_per_sampling_window;

                //set the peak detection threshold for the spectogram
                peak_detection_threshold = config["SensingSubsystemSettings"]["spectogram_peak_detection_threshold_dB"].get<data_type>();
            }


            /**
             * @brief initialize the parameters needed by the fft computation
             * 
             */
            void initialize_fft_params(){
                
                shape = {fft_size};
                stride = {sizeof(std::complex<data_type>)};
                axes = {0};
            }

            /**
             * @brief initialize all buffers used by the spectrogram handler
             * 
             */
            void initialize_buffers(){
                reshaped__and_windowed_signal_for_fft = Buffer_2D<std::complex<data_type>>(num_rows_spectrogram,fft_size);
                
                //window to apply
                hanning_window = Buffer_1D<std::complex<data_type>>(fft_size);
                
                //fft/spectrogram generation
                computed_fft = Buffer_2D<std::complex<data_type>>(num_rows_spectrogram,fft_size);
                generated_spectrogram = Buffer_2D<data_type>(num_rows_spectrogram,fft_size);

                //getting the points from the spectrogram
                spectrogram_points_values = Buffer_1D<data_type>(num_rows_spectrogram);
                spectrogram_points_indicies = Buffer_1D<size_t>(num_rows_spectrogram);

                //tracking detected times
                detected_times = Buffer_1D<data_type>(num_rows_spectrogram);
                detected_frequencies = Buffer_1D<data_type>(num_rows_spectrogram);

                cluster_indicies = Buffer_1D<int>(num_rows_spectrogram);
            }

            void initialize_freq_and_timing_bins(){
                //initialize the frequency parameters and buffers
                frequency_resolution = FMCW_sampling_rate * 1e-6 /
                            static_cast<data_type>(fft_size);

                frequencies = std::vector<data_type>(fft_size,0);

                for (size_t i = 0; i < fft_size; i++)
                {
                    frequencies[i] = frequency_resolution * static_cast<data_type>(i);
                }

                //initialize the timing parameters and buffers
                    //compute the timing offset
                    frequency_sampling_period = 
                            static_cast<data_type>(samples_per_sampling_window)/
                                (FMCW_sampling_rate * 1e-6);
                    
                    detected_time_offset = frequency_sampling_period * 
                                static_cast<data_type>(fft_size) / 2 /
                                static_cast<data_type>(samples_per_sampling_window);
                    
                    //create the times buffer
                    times = std::vector<data_type>(num_rows_spectrogram,0);
                
                    for (size_t i = 0; i < num_rows_spectrogram; i++)
                    {
                        times[i] = (frequency_sampling_period *
                                    static_cast<data_type>(i)) + detected_time_offset;
                    }
            }

            void initialize_clustering_params(){

                // get the minimum number of points per chirp from the JSON file
                min_points_per_chirp = config["SensingSubsystemSettings"]["min_points_per_chirp"].get<size_t>();
            }


            /**
             * @brief Computes a hanning window of a given size for use in the spectogram generation
             * 
             */
            void initialize_hanning_window() {
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
                
                //get dimmensions of received_signal array
                size_t m = received_signal.size(); //rows
                size_t n = received_signal[0].size(); //cols

                //initialize variables for reshaping
                size_t from_r;
                size_t from_c;

                //initialize variable to determine the coordinate in the received signal for a row/col index in reshpaed signal
                size_t k;

                for (size_t i = 0; i < num_rows_spectrogram; i++)
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
              
              //if the end_idx is zero (default condition), set it to be the num_rows_spectrogram
              if (end_idx == 0)
              {
                end_idx = num_rows_spectrogram;
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
                size_t rows_per_thread = num_rows_spectrogram / num_threads;
                size_t start_row;
                size_t end_row;

                //spawn multiple threads
                for (size_t thread_num = 0; thread_num < num_threads; thread_num++)
                {   
                    //determine the start and end row for each thread
                    start_row = thread_num * rows_per_thread;

                    if (thread_num == (num_threads - 1)){
                        end_row = num_rows_spectrogram;
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
             * and saves the results in the detected_times and detected_frequenies array
             * 
             */
            void detect_peaks_in_spectrogram(){
                //initialize variable to store results from compute_max_val
                std::tuple<data_type,size_t> max_val_and_idx;
                data_type max_val;
                size_t idx;

                //variable to track the absolute maximum value detected in the spectrogram
                data_type absolute_max_val = generated_spectrogram.buffer[0][0];

                //clear the detected times and frequencies buffers
                detected_times.clear();
                detected_frequencies.clear();

                //get the maximum_value from each computed_spectrogram
                for (size_t i = 0; i < num_rows_spectrogram; i++)
                {
                    max_val_and_idx = compute_max_val(generated_spectrogram.buffer[i]);
                    max_val = std::get<0>(max_val_and_idx);
                    idx = std::get<1>(max_val_and_idx);
                    spectrogram_points_values.buffer[i] = max_val;
                    spectrogram_points_indicies.buffer[i] = idx;

                    //update the max value
                    if (max_val > absolute_max_val)
                    {
                        absolute_max_val = max_val;
                    }
                }
                
                data_type threshold = absolute_max_val - peak_detection_threshold;
                //go through the spectrogram_points and zero out the points below the threshold
                for (size_t i = 0; i < num_rows_spectrogram; i ++){
                    if (spectrogram_points_values.buffer[i] > threshold)
                    {
                        detected_times.push_back(times[i]);
                        detected_frequencies.push_back(
                            frequencies[
                                spectrogram_points_indicies.buffer[i]]);
                    }
                }
                return;
            }

            /**
             * @brief identify the clusters from the detected times and frequencies
             * 
             */
            void compute_clusters(){

                //declare support variables
                int chirp = 1;
                size_t chirp_start_idx = 0;
                size_t num_points_in_chirp = 1;
                size_t set_val; // use

                //variable to track the total number of detected points
                size_t num_detected_points = detected_frequencies.num_samples;

                //go through the points and determine the clusters
                for (size_t i = 1; i < num_detected_points; i++)
                {
                    if ((detected_frequencies.buffer[i] - detected_frequencies.buffer[i-1]) > 0)
                    {
                        num_points_in_chirp += 1;
                    }
                    else{
                        if(num_points_in_chirp >= min_points_per_chirp){
                            cluster_indicies.set_val_at_indicies(chirp,chirp_start_idx,i);
                            
                            //start tracking the next chirp
                            chirp += 1;
                        }
                        else{
                            cluster_indicies.set_val_at_indicies(-1,chirp_start_idx,i);
                        }

                        //reset support variables for tracking new chirp
                        chirp_start_idx = i;
                        num_points_in_chirp = 1;
                    }
                }

                //check the last point
                if (num_points_in_chirp >= min_points_per_chirp)
                {
                    cluster_indicies.set_val_at_indicies(chirp,chirp_start_idx,num_detected_points);
                }
                else{
                    cluster_indicies.set_val_at_indicies(-1,chirp_start_idx,num_detected_points);
                }
                

                //set the remaining samples in the cluster array to zero
                for (size_t i = num_detected_points; i < num_rows_spectrogram; i++)
                {
                    cluster_indicies.buffer[i] = 0;
                }
            }
        };
    }


#endif
#ifndef ENERGYDETECTOR
#define ENERGYDETECTOR

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

    namespace EnergyDetector_namespace{
        template<typename data_type>
        class EnergyDetector{
            private:
                data_type relative_noise_power;
                data_type threshold_level;
                data_type sampling_frequency;
            public:

            /**
             * @brief Construct a new Energy Detector object
             * 
             * @param threshold the specified threshold in dB that a received signal
             * must be greater than the noise level before the sensing subsystem
             * starts to record samples
             * 
             * @param sampling_frequency the sampling frequency (in seconds) that samples
             * are recorded at
             */
            EnergyDetector(data_type samp_freq, data_type threshold):
                relative_noise_power(0),
                threshold_level(threshold),
                sampling_frequency(samp_freq) {};

            /**
             * @brief Destroy the Energy Detector object
             * 
             */
            ~EnergyDetector () {};

            /**
             * @brief Compute the power of a given rx signal
             * 
             * @param rx_signal the rx signal to compute the power of
             * @return data_type the computed signal power level
             */
            data_type compute_signal_power (std::vector<std::complex<data_type>> & rx_signal){
                
                //get determine the sampling period of the rx_signal
                data_type sampling_period = static_cast<data_type>(rx_signal.size()) / sampling_frequency;
                
                //compute the sum of the elements
                data_type sum;

                for (size_t i = 0; i < rx_signal.size(); i++)
                {
                    sum += ((real(rx_signal[i]) * real(rx_signal[i])) + (imag(rx_signal[i]) * imag(rx_signal[i])));
                }

                //convert to dB and return
                data_type power = 10 * std::log10(sum/sampling_period);
                return power;
            }

            /**
             * @brief Set the relative noise power level which will be used to detect chirps
             * 
             * @param rx_signal the rx signal to sample the noise power level over
             */
            void compute_relative_noise_power(std::vector<std::complex<data_type>> & rx_signal){
                
                //set the relative noise power
                relative_noise_power = compute_signal_power(rx_signal);
                return;
            }

            /**
             * @brief Checks to see if a chirp was detected in the given rx_signal
             * 
             * @param rx_signal the rx_signal
             * @return true - returned if there was a chirp detected
             * @return false - returned if no chirp was detected
             */
            bool check_for_chirp(std::vector<std::complex<data_type>> & rx_signal){

                //compute the rx signal power and determine if it is sufficiently higher than the threshold
                if ((compute_signal_power(rx_signal) - relative_noise_power) >= threshold_level )
                {
                    return true;
                }
                else{
                    return false;
                }
                
            }
        };
    }

#endif
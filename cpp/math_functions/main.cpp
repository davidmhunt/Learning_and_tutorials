#define POCKETFFT_CACHE_SIZE 5

//C standard libraries
#include <iostream>
#include <cstdlib>
#include <string>
#include <chrono>
#include <complex.h>
#include <csignal>
#include <thread>

//including buffer handler
#include "src/BufferHandler.hpp"

//include the JSON Handler
#include <nlohmann/json.hpp>
#include "src/JSONHandler.hpp"

//include the spectrogram handler
#include "src/sensing_subsystem/SpectrogramHandler.hpp"
#include "src/sensing_subsystem/EnergyDetector.hpp"


//include Eigen test code
#include"src/sensing_subsystem/Eigen/Dense"

using SpectrogramHandler_namespace::SpectrogramHandler;
using EnergyDetector_namespace::EnergyDetector;
using namespace std::chrono;
using json = nlohmann::json;


int main(int, char**) {
    
    //get the JSON configuration file
    std::string config_file = "/home/david/Documents/Learning_and_tutorials/cpp/math_functions/config.json";
    json config = JSONHandler::parse_JSON(config_file,false);
    SpectrogramHandler<float> spectrogram_handler(config);

    //initialize the spectrogram_handler
    //SpectrogramHandler<float> spectrogram_handler(994,118,64);
    EnergyDetector<float> energy_detector(config);

    //load a sample received signal to confirm correctness
    std::string path = "/home/david/Documents/MATLAB_generated/MATLAB_received_signal.bin";
    spectrogram_handler.rx_buffer.set_read_file(path,true);
    spectrogram_handler.rx_buffer.import_from_file(1020);
    
    //compute the relative noise power value
    energy_detector.compute_relative_noise_power(spectrogram_handler.rx_buffer.buffer[0]);
    
    //save the hanning window to a file to confirm correctness
    path = "/home/david/Documents/MATLAB_generated/cpp_hanning_window.bin";
    spectrogram_handler.hanning_window.set_write_file(path);
    spectrogram_handler.hanning_window.save_to_file();
    
    //load and reshape the received signal to confirm correctness
    path = "/home/david/Documents/MATLAB_generated/cpp_reshaped_and_windowed_for_fft.bin";
    spectrogram_handler.load_and_prepare_for_fft();
    spectrogram_handler.reshaped__and_windowed_signal_for_fft.set_write_file(path,true);
    spectrogram_handler.reshaped__and_windowed_signal_for_fft.save_to_file();

    //compute the fft to confirm correctness
    path = "/home/david/Documents/MATLAB_generated/cpp_generated_spectrogram.bin";
    spectrogram_handler.compute_ffts();
    spectrogram_handler.generated_spectrogram.set_write_file(path,true);
    spectrogram_handler.generated_spectrogram.save_to_file();

    //detect the points in the spectrogram
    spectrogram_handler.detect_peaks_in_spectrogram();
    path = "/home/david/Documents/MATLAB_generated/cpp_spectrogram_point_vals.bin";
    spectrogram_handler.spectrogram_points_values.set_write_file(path,true);
    spectrogram_handler.spectrogram_points_values.save_to_file();

    //detect the times and frequencies
    path = "/home/david/Documents/MATLAB_generated/cpp_detected_times.bin";
    spectrogram_handler.detected_times.set_write_file(path,true);
    spectrogram_handler.detected_times.save_to_file();
    path = "/home/david/Documents/MATLAB_generated/cpp_detected_frequencies.bin";
    spectrogram_handler.detected_frequencies.set_write_file(path,true);
    spectrogram_handler.detected_frequencies.save_to_file();

    //compute the clusters
    spectrogram_handler.compute_clusters();
    path = "/home/david/Documents/MATLAB_generated/cpp_computed_clusters.bin";
    spectrogram_handler.cluster_indicies.set_write_file(path,true);
    spectrogram_handler.cluster_indicies.save_to_file();

    //fit linear models
    spectrogram_handler.compute_linear_model();
    path = "/home/david/Documents/MATLAB_generated/cpp_detected_slopes.bin";
    spectrogram_handler.detected_slopes.set_write_file(path,true);
    spectrogram_handler.detected_slopes.save_to_file();
    path = "/home/david/Documents/MATLAB_generated/cpp_detected_intercepts.bin";
    spectrogram_handler.detected_intercepts.set_write_file(path,true);
    spectrogram_handler.detected_intercepts.save_to_file();

    //compute victim parameters
    spectrogram_handler.compute_victim_parameters();
    path = "/home/david/Documents/MATLAB_generated/cpp_captured_frames.bin";
    spectrogram_handler.captured_frames.set_write_file(path,true);
    spectrogram_handler.captured_frames.save_to_file();


    double runs = 100;

    auto start = high_resolution_clock::now();
    for (double i = 0; i < runs; i++)
    {
        energy_detector.check_for_chirp(spectrogram_handler.rx_buffer.buffer[1]);
        //spectrogram_handler.load_and_prepare_for_fft();
        //spectrogram_handler.compute_ffts();
        //spectrogram_handler.detect_peaks_in_spectrogram();
        //spectrogram_handler.compute_clusters();
        //spectrogram_handler.compute_linear_model();
        //spectrogram_handler.compute_victim_parameters();
        spectrogram_handler.process_received_signal();
    }

    auto stop = high_resolution_clock::now();
    //auto diff = duration_cast<milliseconds>(stop - start);
    std::chrono::duration<double> diff = stop - start;

    std::cout << "Average time (ms) for " << runs << " runs: " << (diff.count() *1e3)/runs << std::endl;

    return EXIT_SUCCESS;
}

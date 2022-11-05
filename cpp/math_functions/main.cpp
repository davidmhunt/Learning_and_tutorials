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

//include the spectrogram handler
#include "src/sensing_subsystem/SpectrogramHandler.hpp"
#include "src/sensing_subsystem/EnergyDetector.hpp"



using SpectrogramHandler_namespace::SpectrogramHandler;
using EnergyDetector_namespace::EnergyDetector;
using namespace std::chrono;

int main(int, char**) {
    
    //initialize the spectrogram_handler
    SpectrogramHandler<float> spectrogram_handler(994,118,64);
    EnergyDetector<float> energy_detector(5.852e+7,3);

    //load a sample received signal to confirm correctness
    std::string path = "/home/david/Documents/MATLAB_generated/MATLAB_received_signal.bin";
    Buffer_2D<std::complex<float>> received_signal;
    received_signal.set_read_file(path,true);
    received_signal.import_from_file(2040);
    received_signal.print_preview();
    
    //compute the relative noise power value
    energy_detector.compute_relative_noise_power(received_signal.buffer[0]);
    
    //save the hanning window to a file to confirm correctness
    path = "/home/david/Documents/MATLAB_generated/cpp_hanning_window.bin";
    spectrogram_handler.hanning_window.set_write_file(path);
    spectrogram_handler.hanning_window.save_to_file();
    
    //load and reshape the received signal to confirm correctness
    path = "/home/david/Documents/MATLAB_generated/cpp_reshaped_and_windowed_for_fft.bin";
    spectrogram_handler.load_and_prepare_for_fft(received_signal.buffer);
    spectrogram_handler.reshaped__and_windowed_signal_for_fft.set_write_file(path,true);
    spectrogram_handler.reshaped__and_windowed_signal_for_fft.save_to_file();

    //compute the fft to confirm correctness
    path = "/home/david/Documents/MATLAB_generated/cpp_generated_spectrogram.bin";
    spectrogram_handler.compute_ffts_multi_threaded(5);
    spectrogram_handler.generated_spectrogram.set_write_file(path,true);
    spectrogram_handler.generated_spectrogram.save_to_file();

    //detect the points in the spectrogram
    spectrogram_handler.detect_peaks_in_spectrogram();
    path = "/home/david/Documents/MATLAB_generated/cpp_spectrogram_point_vals.bin";
    spectrogram_handler.spectrogram_points_values.set_write_file(path,true);
    spectrogram_handler.spectrogram_points_values.save_to_file();
    path = "/home/david/Documents/MATLAB_generated/cpp_spectrogram_point_indicies.bin";
    spectrogram_handler.spectrogram_points_indicies.set_write_file(path,true);
    spectrogram_handler.spectrogram_points_indicies.save_to_file();


    double runs = 100;

    auto start = high_resolution_clock::now();
    for (double i = 0; i < runs; i++)
    {
        //energy_detector.check_for_chirp(received_signal.buffer[1]);
        //spectrogram_handler.load_and_prepare_for_fft(received_signal.buffer);
        spectrogram_handler.compute_ffts(0);
        //spectrogram_handler.detect_peaks_in_spectrogram();
    }

    auto stop = high_resolution_clock::now();
    //auto diff = duration_cast<milliseconds>(stop - start);
    std::chrono::duration<double> diff = stop - start;

    std::cout << "Average time (ms) for " << runs << " runs: " << (diff.count() *1e3)/runs << std::endl;

/*
    // initialize the buffers to load the signal and save the processed signal
    std::string sample_sinusoid_path = "/home/david/Documents/MATLAB_generated/sample_sinusoid.bin";
    Buffer_1D<std::complex<float>> sample_sinusoid;
    sample_sinusoid.set_read_file(sample_sinusoid_path);
    sample_sinusoid.import_from_file();
    sample_sinusoid.print_preview();
    
    std::string fft_save_path = "/home/david/Documents/MATLAB_generated/computed_fft.bin";
    Buffer_1D<std::complex<float>> computed_fft;
    computed_fft.set_write_file(fft_save_path);
    computed_fft.set_buffer_size(sample_sinusoid.num_samples);
    

    //perform the fft
    shape_t shape{sample_sinusoid.num_samples};
    stride_t stride{sizeof(std::complex<float>)};
    shape_t axes{0};

    //define the number of runs to get an idea of the run time
    size_t runs = 1000;

    //allocate another two vectors to test multi-threaded support
    std::vector<std::complex<float>> read_buff1 = sample_sinusoid.buffer;
    std::vector<std::complex<float>> read_buff2 = sample_sinusoid.buffer;
    std::vector<std::complex<float>> read_buff3 = sample_sinusoid.buffer;
    std::vector<std::complex<float>> write_buff1 = computed_fft.buffer;
    std::vector<std::complex<float>> write_buff2 = computed_fft.buffer;
    std::vector<std::complex<float>> write_buff3 = computed_fft.buffer;

    auto start = high_resolution_clock::now();

    //create 2 additional threads
    std::thread thread1([&]() {
        for (size_t i = 0; i < runs/4; i++)
        {
            c2c(shape, stride, stride, axes, FORWARD,
                read_buff1.data(), write_buff1.data(), (float) 1.);
        }
    });

    std::thread thread2([&]() {
        for (size_t i = 0; i < runs/4; i++)
        {
            c2c(shape, stride, stride, axes, FORWARD,
                read_buff2.data(), write_buff2.data(), (float) 1.0);
        }
    });

    std::thread thread3([&]() {
        for (size_t i = 0; i < runs/4; i++)
        {
            c2c(shape, stride, stride, axes, FORWARD,
                read_buff3.data(), write_buff3.data(), (float) 1.);
        }
    });

    for (size_t j = 0; j < runs/4; j++)
    {
        c2c(shape, stride, stride, axes, FORWARD,
            sample_sinusoid.buffer.data(), computed_fft.buffer.data(), (float) 1.);
    }

    //wait for victim thread to finish
    thread1.join();
    thread2.join();
    thread3.join();
    
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);

    std::cout << "Total fft time (ms) for " << runs << " runs: " << duration.count() << std::endl;
    

    


    //save the computed fft and save it to the write file
    computed_fft.save_to_file();

*/
    return EXIT_SUCCESS;
}

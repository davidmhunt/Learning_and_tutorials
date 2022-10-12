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

//include the pocketfft computation method
#include "pocketfft/pocketfft_hdronly.h"



using Buffers::Buffer_1D;
using namespace pocketfft;
using namespace std::chrono;

int main(int, char**) {
    
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
    size_t runs = 4000;

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
                read_buff1.data(), write_buff1.data(), 1.f);
        }
    });

    std::thread thread2([&]() {
        for (size_t i = 0; i < runs/4; i++)
        {
            c2c(shape, stride, stride, axes, FORWARD,
                read_buff2.data(), write_buff2.data(), 1.f);
        }
    });

    std::thread thread3([&]() {
        for (size_t i = 0; i < runs/4; i++)
        {
            c2c(shape, stride, stride, axes, FORWARD,
                read_buff3.data(), write_buff3.data(), 1.f);
        }
    });

    for (size_t j = 0; j < runs/4; j++)
    {
        c2c(shape, stride, stride, axes, FORWARD,
            sample_sinusoid.buffer.data(), computed_fft.buffer.data(), 1.f);
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

    return EXIT_SUCCESS;
}

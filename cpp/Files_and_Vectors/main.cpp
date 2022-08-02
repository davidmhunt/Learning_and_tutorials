/*
    Description: Includes code used to read/write a 2D vector containing complex numbers to a file. 
                    Ideally, this will allow the user to stream data on C++ and then save the data for offline processing later in MATLAB
*/
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <complex>
#include <string>
#include <vector>

int main(void) {
    /*
        CODE FOR WRITING A FILE
    */
    
   std::cout << "WRITING A FILE\n\n";

    //define a 2d write vector
    size_t num_rows = 4;
    size_t num_cols = 5;
    size_t num_frames = 3;
    std::vector<std::vector<std::complex<float>>> write_vector (num_rows,std::vector<std::complex<float>>(num_cols));

    //initialize the file writing
    std::string write_file_name("/home/david/Documents/MATLAB_generated/write.bin");
    std::ofstream write_file_stream;
    write_file_stream.open(write_file_name.c_str(), std::ios::out | std::ios::binary);
    
    if(write_file_stream.is_open()){
        for (size_t frame = 0; frame < num_frames; frame++)
        {
            float frame_inc = 1 + (static_cast<float>(frame) + 1) * 0.001;
            //loop through and assign values
            for (size_t i = 0; i < num_rows; i++)
            {
                for (size_t j = 0; j < num_cols; j++)
                {
                    write_vector[i][j] = std::complex<float>(static_cast<float>(i) + frame_inc,static_cast<float>(j) + frame_inc);
                }
            } 
            
            //write to the file
            for (size_t i = 0; i < num_rows; i++)
            {
                write_file_stream.write((char*) &write_vector[i].front(), write_vector[i].size() * sizeof(std::complex<float>));
            }

            

            //Print the binary number as a test run
            for (size_t i = 0; i < num_rows; i++)
            {
                for (size_t j = 0; j < num_cols; j++)
                {
                    std::cout << write_vector[i][j].real() << " + " << write_vector[i][j].imag() << "j, ";
                }
                std::cout << std::endl;
            }
            std::cout << "\n\n";
        }
        //close the file
        write_file_stream.close();
    }
    else{
        std::cout << "failed to open write file \n";
    }
    


    /*
        CODE FOR Reading A FILE
    */
    
   std::cout << "\n\nREADING A FILE\n\n";

    //initialize the file reading
    std::string read_file_name("/home/david/Documents/MATLAB_generated/chirp_full.bin");

    std::ifstream read_file_stream;
    read_file_stream.open(read_file_name.c_str(), std::ios::in | std::ios::binary);

    if (read_file_stream.is_open()){

        //get the size of the file to be read
        std::streampos size;
        read_file_stream.seekg (0,std::ios::end);
        size = read_file_stream.tellg();

        //determine the number of samples in the file
        size_t num_samps = size / sizeof(std::complex<float>);
        std::cout << "Num samples detected: " << num_samps << std::endl;

        //define the complex vector
        std::vector<std::complex<float>> read_vector (num_samps);

        //read the file
        read_file_stream.seekg(0,std::ios::beg);
        read_file_stream.read((char*) &read_vector.front(), read_vector.size() * sizeof(std::complex<float>));
        read_file_stream.close();

        //print a preview of the file that was read
        std::cout << "Read File: \n";
        for (size_t i = 0; i < 5; i++)
        {
            std::cout << read_vector[i].real() << " + " << read_vector[i].imag() << "j, ";
        }
        std::cout << "\t...\t" << read_vector.back().real() << " + " << read_vector.back().imag() << "j" <<std::endl;
        

    }
    else{
        std::cout << "Could not open file\n";
    }

    /*
        CODE for resizinga vector
    */
    
   std::cout << "Resizing a vector\n\n";

    //create a sample chirp
    size_t samples_per_chirp = 10;
    std::vector<std::complex<float>> chirp(samples_per_chirp,std::complex<float>(0,0));
    for (size_t i = 0; i < samples_per_chirp; i++)
    {
        chirp[i] = std::complex<float>(static_cast<float>(i) +1,0);
    }
    
    //store the chirp in buffers as big as the largest samples per buffer size
    size_t samples_per_buff = 5;
    size_t num_chirps = 2;

    //compute the numer of buffers we need
    //size_t num_rows; //number of buffers to send
    size_t excess_samples; //the number of extra samples that will be sent

    if (samples_per_buff == samples_per_chirp){
        num_rows = num_chirps;
        excess_samples = 0;
    }
    else if (((num_chirps * samples_per_chirp) % samples_per_buff) == 0){
        num_rows = (num_chirps * samples_per_chirp) / samples_per_buff;
        excess_samples = 0;   
    }
    else
    {
        num_rows = ((num_chirps * samples_per_chirp) / samples_per_buff) + 1;
        excess_samples = (num_rows * samples_per_buff) - (num_chirps * samples_per_chirp);
    }
    std::cout << "Num Rows: " << num_rows << " Excess Samples: " << excess_samples << std::endl;
    
    //copy the chirp into the buffers
    std::vector<std::vector<std::complex<float>>> tx_buffer (num_rows,std::vector<std::complex<float>>(samples_per_buff));
    std::vector<std::complex<float>>::iterator chirp_iterator = chirp.begin();
    size_t row = 0;
    std::vector<std::complex<float>>::iterator buffer_iterator = tx_buffer[0].begin();
    while (buffer_iterator != (tx_buffer[num_rows - 1].end() - excess_samples))
    {
        *buffer_iterator = *chirp_iterator;

        //increment chirp iterator
        if(chirp_iterator == chirp.end() - 1){
            chirp_iterator = chirp.begin();
        }
        else{
            ++chirp_iterator;
        }

        //increment buffer iterator
        if(buffer_iterator == tx_buffer[row].end() - 1){
            if(row == (num_rows - 1) && excess_samples == 0){
                buffer_iterator = tx_buffer[row].end();
            }
            else{
                row = row + 1;
                buffer_iterator = tx_buffer[row].begin();
            }
        }
        else{
            ++buffer_iterator;
        }
    }
    
    //Print the tx_buffer
    for (size_t i = 0; i < num_rows; i++)
    {
        for (size_t j = 0; j < tx_buffer[i].size(); j++)
        {
            std::cout << tx_buffer[i][j].real() << " + " << tx_buffer[i][j].imag() << "j, ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n\n";
    
    return EXIT_SUCCESS;
}

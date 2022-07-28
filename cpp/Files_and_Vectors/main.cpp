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
    
    return EXIT_SUCCESS;
}

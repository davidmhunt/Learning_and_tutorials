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

    //define the complex number
    size_t num_rows = 4;
    std::vector<std::complex<float>> write_vector (num_rows);

    //loop through and assign values
    for (size_t i = 0; i < num_rows; i++)
    {
        write_vector[i] = std::complex<float>(1.001,static_cast<float>(i) +1);
    }
    
    
    //initialize the file writing
    std::string write_file_name("/home/david/Documents/MATLAB_generated/write.bin");
    std::ofstream write_file_stream;
    write_file_stream.open(write_file_name.c_str(), std::ios::out | std::ios::binary);
    write_file_stream.write((char*) &write_vector.front(), write_vector.size() * sizeof(std::complex<float>));
    write_file_stream.close();

    

    //Print the binary number as a test run
    for (size_t i = 0; i < num_rows; i++)
    {
        std::cout << write_vector[i].real() << " + " << write_vector[i].imag() << "j, ";
    }
    std::cout << std::endl;


    /*
        CODE FOR Reading A FILE
    */
    
   std::cout << "\n\nREADING A FILE\n\n";

    //initialize the file reading
    std::string read_file_name("/home/david/Documents/MATLAB_generated/write.bin");
    std::ifstream read_file_stream;
    read_file_stream.open(write_file_name.c_str(), std::ios::in | std::ios::binary);
    read_file_stream.read((char*) &write_vector.front(), write_vector.size() * sizeof(std::complex<float>));
    read_file_stream.close();

    //define the complex number
    size_t num_rows = 4;
    std::vector<std::complex<float>> write_vector (num_rows);

    //loop through and assign values
    for (size_t i = 0; i < num_rows; i++)
    {
        write_vector[i] = std::complex<float>(1.001,static_cast<float>(i) +1);
    }
    
    
    

    

    //Print the binary number as a test run
    for (size_t i = 0; i < num_rows; i++)
    {
        std::cout << write_vector[i].real() << " + " << write_vector[i].imag() << "j, ";
    }
    std::cout << std::endl;

    return EXIT_SUCCESS;
}

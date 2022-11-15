cmake_minimum_required(VERSION 3.0.0)
project(math_functions VERSION 0.1.0)

include(CTest)
enable_testing()

add_subdirectory(src)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(nlohmann_json 3.10.5 REQUIRED)

#add support for fftw - a few other things are needed as well
#find_package(PkgConfig REQUIRED)
#pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
#include_directories(PkgConfig::FFTW)
#link_libraries     (PkgConfig::FFTW)



add_executable(math_functions main.cpp)

#perform linking
target_link_libraries(math_functions    PRIVATE BufferHandler
                                        PRIVATE JSONHandler
                                        PUBLIC Threads::Threads
                                        PRIVATE SpectrogramHandler
                                        PRIVATE EnergyDetector
                                        PRIVATE nlohmann_json::nlohmann_json)

#include directories
target_link_directories(math_functions PRIVATE 
                        ${PROJECT_BINARY_DIR}
                        ${PROJECT_BINARY_DIR}/src
                        ${PROJECT_BINARY_DIR}/src/sensing_subsystem
                        ${PROJECT_BINARY_DIR}/src/sensing_subsystem/pocketfft
                        ${PROJECT_BINARY_DIR}/src/sensing_subsystem/Eigen
                        )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

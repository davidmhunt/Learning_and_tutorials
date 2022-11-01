#enable threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#declare libraries
add_library(SpectrogramHandler SpectrogramHandler.cpp)
add_library(EnergyDetector EnergyDetector.cpp)


#link libraries as needed
target_link_libraries(SpectrogramHandler PRIVATE Threads::Threads)
target_link_libraries(SpectrogramHandler PRIVATE BufferHandler)

#include directories
target_link_directories(SpectrogramHandler PRIVATE 
                        ${WORKING_DIRECTORY}/pocketfft
                        ${PARENT_DIRECTORY}
                        )
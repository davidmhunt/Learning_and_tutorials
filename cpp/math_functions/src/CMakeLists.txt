#add subdirectories/dependencies
find_package(nlohmann_json 3.10.5 REQUIRED)

#enable threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#declare libraries
add_library(BufferHandler BufferHandler.cpp)
add_library(JSONHandler JSONHandler.cpp)

#add in the sensing subsystem libraries - make sure that buffer handler is built before
add_subdirectory(sensing_subsystem)


#link libraries as needed
target_link_libraries(BufferHandler PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(JSONHandler PRIVATE nlohmann_json::nlohmann_json)
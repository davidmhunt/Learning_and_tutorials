#add subdirectories/dependencies
find_package(nlohmann_json 3.10.5 REQUIRED)

#declare libraries
add_library(BufferHandler BufferHandler.cpp)


#link libraries as needed
target_link_libraries(BufferHandler PRIVATE nlohmann_json::nlohmann_json)
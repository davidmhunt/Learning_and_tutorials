#add the subdirectory/dependencies
find_package(nlohmann_json 3.10.5 REQUIRED)

#declare the library
add_library(parseJSON parseJson.cpp)

#link the library
target_link_libraries(parseJSON PRIVATE nlohmann_json::nlohmann_json)
cmake_minimum_required(VERSION 3.0.0)
project(JSON_Parse_TEST VERSION 0.1.0)

include(CTest)
enable_testing()

#add required libraries and subdirectories
find_package(nlohmann_json 3.10.5 REQUIRED)
add_subdirectory(parseJSON)

#add the executable
add_executable(JSON_Parse_TEST main.cpp)

#link the libraries together
target_link_libraries(JSON_Parse_TEST PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(JSON_Parse_TEST PUBLIC parseJSON)

#include directories
target_include_directories(JSON_Parse_TEST PUBLIC 
                            ${PROJECT_BINARY_DIR}
                            ${PROJECT_SOURCE_DIR}/parseJSON)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

cmake_minimum_required(VERSION 3.0.0)
project(JSON_Decoding VERSION 0.1.0)

include(CTest)
enable_testing()

#if using the JSON editor at a higher level, will need to call the find_packages command here
find_package(nlohmann_json 3.10.5 REQUIRED)

add_subdirectory(modules)

add_executable(JSON_Decoding main.cpp)
target_link_libraries(JSON_Decoding PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(JSON_Decoding PRIVATE modules_lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

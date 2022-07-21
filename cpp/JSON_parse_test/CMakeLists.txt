cmake_minimum_required(VERSION 3.0.0)
project(JSON_Parse VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(nlohmann_json 3.10.5 REQUIRED)
add_executable(JSON_Parse main.cpp)
target_link_libraries(JSON_Parse PRIVATE nlohmann_json::nlohmann_json)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

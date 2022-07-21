add_library(
    modules_lib
    get_meta/get_meta_data.cpp
    get_meta/get_meta_data.hpp
)

find_package(nlohmann_json 3.10.5 REQUIRED)
target_link_libraries(modules_lib PUBLIC nlohmann_json::nlohmann_json)

target_include_directories(modules_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
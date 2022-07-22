cmake_minimum_required(VERSION 3.0.0)
project(UHD_find_devices VERSION 0.1.0)

include(CTest)
enable_testing()

#commands included from the uhd github website
set(CMAKE_CXX_STANDARD 11)

if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_EXE_LINKER_FLAGS "-lthr ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}")
endif()

option(UHD_USE_STATIC_LIBS OFF)

find_package(UHD 4.1.0 REQUIRED)

set(UHD_BOOST_REQUIRED_COMPONENTS
    program_options
    system
    thread
)
set(BOOST_MIN_VERSION 1.65)
include(UHDBoost)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${UHD_INCLUDE_DIRS}
)
link_directories(${Boost_LIBRARY_DIRS})
#end of code from uhd github website

add_executable(UHD_find_devices main.cpp)

#link the uhd editor
target_link_libraries(UHD_find_devices ${UHD_LIBRARIES} ${BoostLIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

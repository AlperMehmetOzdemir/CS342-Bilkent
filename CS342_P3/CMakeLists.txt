cmake_minimum_required(VERSION 3.6)
project(CS342_P2)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES pmatch3.c)
add_executable(CS342_P2 ${SOURCE_FILES})
find_package (Threads)
target_link_libraries (CS342_P2 ${CMAKE_THREAD_LIBS_INIT})
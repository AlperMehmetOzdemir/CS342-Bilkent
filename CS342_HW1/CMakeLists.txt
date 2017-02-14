cmake_minimum_required(VERSION 3.6)
project(CS342_HW1)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES btree.c)
add_executable(CS342_HW1 ${SOURCE_FILES})
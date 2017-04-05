cmake_minimum_required(VERSION 3.7)
project(CS342_P3)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c diningphil.h probabilityengine.h)
add_executable(CS342_P3 ${SOURCE_FILES} diningphil.h probabilityengine.h)
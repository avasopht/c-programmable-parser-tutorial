cmake_minimum_required(VERSION 3.10)

project(c-programmable-parser-tutorial)
set(CMAKE_C_STANDARD 11)
set(FILES parser.c)
add_executable(c-programmable-parser ${FILES})
cmake_minimum_required(VERSION 3.20)
project(1_random_numbers C)

set(CMAKE_C_STANDARD 99)

add_executable(1_random_numbers main.c)

target_link_libraries(1_random_numbers PRIVATE m)

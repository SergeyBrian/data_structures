cmake_minimum_required(VERSION 3.21)
project(2_database C)

set(CMAKE_C_STANDARD 99)

add_executable(2_database main.c db.c db.h utils.c utils.h list.c list.h)

cmake_minimum_required(VERSION 3.24)
project(4_hashtable C)

set(CMAKE_C_STANDARD 11)

add_executable(4_hashtable main.c hashtable.c hashtable.h node.c node.h adler32.c adler32.h)

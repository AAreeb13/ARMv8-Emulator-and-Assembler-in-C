cmake_minimum_required(VERSION 3.23)
project(src C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(src
        assemble.c
        dataProcessImmediate.c
        dataProcessImmediate.h
        dataProcessRegister.c
        dataProcessRegister.h
        structures.c
        structures.h
        utils.c
        utils.h)

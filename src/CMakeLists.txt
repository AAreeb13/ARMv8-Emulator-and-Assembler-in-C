cmake_minimum_required(VERSION 3.23)
project(src C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(src
        aliases.h
        aliases.c
        assemble.c
        assemble.h
        branching.h
        branching.c
        dataProcess.c
        dataProcess.h
        dataProcessImmediate.c
        dataProcessImmediate.h
        dataProcessRegister.c
        dataProcessRegister.h
        dataTransfer.c
        dataTransfer.h
        structures.c
        structures.h
        utils.c
        utils.h)

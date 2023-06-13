cmake_minimum_required(VERSION 3.23)
project(C_Project_Final C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(C_Project_Final
        branch.c
        branch.h
        dataProcessImmediate.c
        dataProcessImmediate.h
        dataProcessRegister.c
        dataProcessRegister.h
        emulate.c
        misc.c
        misc.h
        storeandLoad.c
        storeandLoad.h
        structure.c
        structure.h)

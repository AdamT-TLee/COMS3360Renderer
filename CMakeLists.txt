cmake_minimum_required(VERSION 4.0)
project(COMS3360Renderer)

set(CMAKE_CXX_STANDARD 20)

add_executable(COMS3360Renderer main.cpp
        vec3.h
        color.h
        ray.h
        )

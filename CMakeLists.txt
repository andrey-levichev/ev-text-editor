cmake_minimum_required(VERSION 3.6)
project(eve)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    eve/console.cpp
    eve/console.h
    eve/eve.cpp
    eve/eve.h
    eve/file.cpp
    eve/file.h
    eve/foundation.cpp
    eve/foundation.h
    eve/test.cpp
    eve/test.h
    eve/todo.txt)

include_directories(eve)

add_executable(eve ${SOURCE_FILES})
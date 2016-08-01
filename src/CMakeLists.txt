cmake_minimum_required (VERSION 2.8)
project (MINAMI C CXX)

find_package(Threads)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_COMPILER g++)
set (CMAKE_CXX_FLAGS "-fopenmp -std=c++11 -m64 -Wall -pthread -O3")
set (CMAKE_EXE_LIKKER_FLAGS "-pthread")


add_subdirectory (glib-core)
add_subdirectory (snap-core)
add_subdirectory (3rdparty)
add_subdirectory (minami)



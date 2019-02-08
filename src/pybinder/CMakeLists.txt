#cmake_minimum_required(VERSION 2.8.12)

add_subdirectory(pybind11)

file(GLOB_RECURSE mimamiapi_src "src/*.cpp" "src/*.h")

pybind11_add_module(MNMAPI ${mimamiapi_src})
target_link_libraries (MNMAPI Snap minami Eigen3::Eigen)

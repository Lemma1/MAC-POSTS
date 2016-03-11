add_library (minami SHARED path.cpp)

target_link_libraries (minami LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

add_executable (pathDemo test_path.cpp)


target_link_libraries (pathDemo LINK_PUBLIC Snap g3log minami ${CMAKE_THREAD_INIT})

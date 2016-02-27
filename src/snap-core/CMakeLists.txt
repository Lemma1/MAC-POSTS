add_library (Snap SHARED Snap.cpp)

target_link_libraries (Snap LINK_PUBLIC Glib)
target_include_directories (Snap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

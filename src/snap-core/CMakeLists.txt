add_library (Snap STATIC Snap.cpp)

target_link_libraries (Snap Glib)

target_include_directories (Snap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

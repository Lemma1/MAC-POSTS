file(GLOB_RECURSE mimami_src "*.cpp" "*.h")

add_library (minami SHARED ${mimami_src})

target_link_libraries (minami Snap g3log adv_ds)

target_include_directories (minami PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

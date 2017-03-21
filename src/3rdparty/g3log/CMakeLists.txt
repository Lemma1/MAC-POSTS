file( GLOB LIB_SOURCES *.cpp )
add_library(g3log  ${LIB_SOURCES} )

target_include_directories (g3log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

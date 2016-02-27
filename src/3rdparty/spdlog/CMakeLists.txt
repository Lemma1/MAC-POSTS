add_library (spdlog SHARED spdlog.h)

SET_TARGET_PROPERTIES(spdlog PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories (spdlog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library (adv_ds SHARED union_find.h radix_heap.h bimap.h)

SET_TARGET_PROPERTIES(adv_ds PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories (adv_ds PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

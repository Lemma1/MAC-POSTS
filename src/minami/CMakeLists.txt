#add_library (minami SHARED base.h)

#target_link_libraries (minami LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (pathDemo main.cpp)

#target_link_libraries (pathDemo LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (ctm test_ctm.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp dta.cpp)

add_executable (dta dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp)
target_link_libraries (dta LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

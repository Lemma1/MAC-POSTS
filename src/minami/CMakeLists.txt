#add_library (minami SHARED base.h)

#target_link_libraries (minami LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (pathDemo main.cpp)

#target_link_libraries (pathDemo LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (ctm test_ctm.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp dta.cpp)

add_executable (dta_response test_dta.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp path.cpp vms.cpp workzone.cpp)

target_link_libraries (dta_response LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (path test_path.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp)

#target_link_libraries (path LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

add_executable (realtime test_realtime.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp realtime_dta.cpp marginal_cost.cpp vms.cpp workzone.cpp)

target_link_libraries (realtime LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (server realtime_dta_server.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp realtime_dta.cpp marginal_cost.cpp vms.cpp)

#target_link_libraries (server LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

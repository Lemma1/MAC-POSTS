#add_executable (pathDemo main.cpp)

#target_link_libraries (pathDemo LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (ctm test_ctm.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp dta.cpp)

add_executable (dta_response test_dta.cpp)

target_link_libraries (dta_response PUBLIC Snap g3log minami ${CMAKE_THREAD_INIT})

# add_executable (path test_path.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp)

#target_link_libraries (path LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (realtime test_realtime.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp realtime_dta.cpp marginal_cost.cpp vms.cpp workzone.cpp)

#target_link_libraries (realtime LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (server realtime_dta_server.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp realtime_dta.cpp marginal_cost.cpp vms.cpp)

#target_link_libraries (server LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (test_ults test_ults.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp realtime_dta.cpp marginal_cost.cpp vms.cpp workzone.cpp)

#target_link_libraries (test_ults LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (exp_realtime exp_realtime.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp realtime_dta.cpp marginal_cost.cpp vms.cpp workzone.cpp)

#target_link_libraries (exp_realtime LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

# add_executable (test_sp test_sp.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp realtime_dta.cpp marginal_cost.cpp vms.cpp workzone.cpp)

# target_link_libraries (test_sp LINK_PUBLIC Snap ${CMAKE_THREAD_INIT})

# add_executable (test1 test.cpp)
# target_link_libraries (test1 LINK_PUBLIC Snap ${CMAKE_THREAD_INIT})

# add_executable (testGlib test_Glib.cpp)
# target_link_libraries (testGlib PUBLIC Snap minami)

#add_executable (testCC test_cumulative_curve.cpp)
#target_link_libraries (testCC PUBLIC Snap minami)


#add_executable (exp_link exp_link.cpp)
#target_link_libraries (exp_link PUBLIC Snap minami)

#add_executable (test_emission test_emission.cpp)
#target_link_libraries (test_emission PUBLIC Snap minami)

#add_executable (test_misc test_misc.cpp)
#target_link_libraries (test_misc PUBLIC Snap minami)

#add_executable (test_ksp test_ksp.cpp)
#target_link_libraries (test_ksp PUBLIC Snap minami adv_ds)
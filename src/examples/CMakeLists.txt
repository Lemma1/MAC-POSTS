#add_executable (pathDemo main.cpp)

#target_link_libraries (pathDemo LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (ctm test_ctm.cpp)

#target_link_libraries (ctm Snap g3log minami ${CMAKE_THREAD_INIT})


#add_executable (dta_response test_dta.cpp)

#target_link_libraries (dta_response Snap g3log minami ${CMAKE_THREAD_INIT})

add_executable (path test_path.cpp)

target_link_libraries (path PUBLIC Snap minami adv_ds)

#add_executable (realtime test_realtime.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp realtime_dta.cpp marginal_cost.cpp vms.cpp workzone.cpp)

#target_link_libraries (realtime LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (server realtime_dta_server.cpp )

#target_link_libraries (server Snap g3log minami ${CMAKE_THREAD_INIT})

#add_executable (test_ults test_ults.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp realtime_dta.cpp marginal_cost.cpp vms.cpp workzone.cpp)

#target_link_libraries (test_ults LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (exp_realtime exp_realtime.cpp path.cpp dta.cpp vehicle.cpp dlink.cpp od.cpp factory.cpp dnode.cpp ults.cpp io.cpp routing.cpp statistics.cpp shortest_path.cpp realtime_dta.cpp marginal_cost.cpp vms.cpp workzone.cpp)

#target_link_libraries (exp_realtime LINK_PUBLIC Snap g3log ${CMAKE_THREAD_INIT})

#add_executable (test_sp test_sp.cpp )

#target_link_libraries (test_sp LINK_PUBLIC minami Snap ${CMAKE_THREAD_INIT})

# add_executable (test1 test.cpp)
# target_link_libraries (test1 LINK_PUBLIC Snap ${CMAKE_THREAD_INIT})

#add_executable (testGlib test_Glib.cpp)
#target_link_libraries (testGlib PUBLIC Snap)

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

#add_executable (test_tdsp test_tdsp.cpp)
#target_link_libraries (test_tdsp Snap minami adv_ds)

#add_executable (test_due test_due.cpp)
#target_link_libraries (test_due Snap minami adv_ds)

#add_executable (test_multiclass test_multiclass.cpp)
#target_link_libraries (test_multiclass Snap minami adv_ds)

#add_executable (test_multiclass_debug test_multiclass_debug.cpp)
#target_link_libraries (test_multiclass_debug Snap minami adv_ds)

#add_executable (test_detNL test_determined_routing.cpp)
#target_link_libraries (test_detNL Snap minami adv_ds)

#add_executable (test_detNL test_determined_routing.cpp)
#target_link_libraries (test_detNL Snap minami adv_ds)

#add_executable (test_dta_gradient test_dta_gradient.cpp)
#target_link_libraries (test_dta_gradient Snap minami adv_ds)

add_executable (test_tree_cc test_tree_cc.cpp)
target_link_libraries (test_tree_cc Snap minami adv_ds)

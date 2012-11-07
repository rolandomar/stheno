
IF(BUILD_SHARED STREQUAL "TRUE")
        add_executable(cellid_test DiscoveryQuery_test.cpp CellID_testrunner.cpp)
        TARGET_LINK_LIBRARIES(cellid_test superpeer_plugin stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread cppunit)
	add_test(cellid_test ${EXECUTABLE_OUTPUT_PATH}/cellid_test)

        add_executable(DiscoveryQuery_test DiscoveryQuery_test.cpp DiscoveryQuery_testrunner.cpp)
        TARGET_LINK_LIBRARIES(DiscoveryQuery_test superpeer_plugin stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread cppunit)
	add_test(cellid_test ${EXECUTABLE_OUTPUT_PATH}/DiscoveryQuery_test)
ENDIF(BUILD_SHARED STREQUAL "TRUE")



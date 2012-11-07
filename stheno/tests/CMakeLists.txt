
IF(BUILD_SHARED STREQUAL "TRUE")
        add_executable(runtime_test_binary Runtime.cpp RuntimeTestRunner.cpp)
        TARGET_LINK_LIBRARIES(runtime_test_binary stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread cppunit)
	add_test(runtime_test ${EXECUTABLE_OUTPUT_PATH}/runtime_test_binary)
ENDIF(BUILD_SHARED STREQUAL "TRUE")


IF(BUILD_SHARED STREQUAL "TRUE")
        add_executable(servicemanagement_test ServiceManagement.cpp ServiceManagementTestRunner.cpp)
        TARGET_LINK_LIBRARIES(servicemanagement_test stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread cppunit)
	add_test(servicemanagement_test ${EXECUTABLE_OUTPUT_PATH}/servicemanagement_test)
ENDIF(BUILD_SHARED STREQUAL "TRUE")

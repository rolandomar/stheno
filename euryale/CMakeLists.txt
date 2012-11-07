PROJECT(euryale)
cmake_minimum_required(VERSION 2.8)
INCLUDE(../config.cmake)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	#ADD_DEFINITIONS(-g)
	SET(LIBRARY_OUTPUT_PATH ${euryale_BINARY_DIR}/Debug)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Debug)
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
	ADD_DEFINITIONS(-O3)
	SET(LIBRARY_OUTPUT_PATH ${euryale_BINARY_DIR}/Release)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Release)
ELSE(CMAKE_BUILD_TYPE STREQUAL "Release")
#	ADD_DEFINITIONS(-g)
	SET(LIBRARY_OUTPUT_PATH ${euryale_BINARY_DIR}/Debug)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Debug)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")


#Euryale
set(euryale_base src/org/cracs/euryale)

# Common
set(euryale_common 					${euryale_base}/common)
AUX_SOURCE_DIRECTORY(${euryale_base} 			euryale_src_base)
AUX_SOURCE_DIRECTORY(${euryale_common}			euryale_src_common)
AUX_SOURCE_DIRECTORY(${euryale_common}/io 		euryale_src_common_io)
AUX_SOURCE_DIRECTORY(${euryale_common}/sleep 		euryale_src_common_sleep)
#AUX_SOURCE_DIRECTORY(${euryale_common}/uuid 		euryale_src_common_uuid)
SET(euryale_src_common_uuid                             ${euryale_common}/uuid/UUID.cpp ${euryale_common}/uuid/UUIDDistance.cpp ${euryale_common}/uuid/PeerLocation.cpp)
SET(euryale_src_common_uuid_generator                   ${euryale_common}/uuid/uuid_generator.cpp)
AUX_SOURCE_DIRECTORY(${euryale_common}/exception 	euryale_src_common_exception)
AUX_SOURCE_DIRECTORY(${euryale_common}/exception/system euryale_src_common_exception_system)

AUX_SOURCE_DIRECTORY(${euryale_common}/sm               euryale_src_common_sm)
AUX_SOURCE_DIRECTORY(${euryale_common}/sm/exception     euryale_src_common_sm_exception)

set(euryale_package_common
${euryale_src_base}
${euryale_src_common}
${euryale_src_common_io}
${euryale_src_common_uuid}
${euryale_src_common_sleep}
${euryale_src_common_exception}
${euryale_src_common_exception_system}
${euryale_src_common_sm}
${euryale_src_common_sm_exception}
)
# End Common

set(euryale_timer					${euryale_base}/timer)
AUX_SOURCE_DIRECTORY(${euryale_timer} 		euryale_src_timer)
set(euryale_package_timer ${euryale_src_timer})


# Threading
set(euryale_threading 					${euryale_base}/threading)
AUX_SOURCE_DIRECTORY(${euryale_threading} 		euryale_src_threading)
AUX_SOURCE_DIRECTORY(${euryale_threading}/exception 	euryale_src_threading_exception)
AUX_SOURCE_DIRECTORY(${euryale_threading}/tp 		euryale_src_threading_tp)
AUX_SOURCE_DIRECTORY(${euryale_threading}/tp/lf 	euryale_src_threading_tp_lf)
AUX_SOURCE_DIRECTORY(${euryale_threading}/tpc 		euryale_src_threading_tpc)
AUX_SOURCE_DIRECTORY(${euryale_threading}/tpr 		euryale_src_threading_tpr)
set(euryale_package_threading ${euryale_src_threading} ${euryale_src_threading_tp} ${euryale_src_threading_tp_lf} ${euryale_src_threading_tpc} ${euryale_src_threading_tpr}
${euryale_src_threading_exception})
# End Threading

# Tools
set(euryale_tools 					${euryale_base}/tools)
AUX_SOURCE_DIRECTORY(${euryale_tools} 			euryale_src_tools)
AUX_SOURCE_DIRECTORY(${euryale_tools}/loader            euryale_src_tools_loader)
SET(euryale_src_tools_loader                            ${euryale_base}/tools/loader/Loader.cpp)
SET(euryale_src_tools_loader_load                       ${euryale_base}/tools/loader/load.cpp)
AUX_SOURCE_DIRECTORY(${euryale_tools}/monitoring	euryale_src_tools_monitoring)
set(euryale_package_tools ${euryale_src_tools} ${euryale_src_tools_loader} ${euryale_src_tools_monitoring})

# Event
set(euryale_event 					${euryale_base}/event)
AUX_SOURCE_DIRECTORY(${euryale_event} 			euryale_src_event)
set(euryale_package_event ${euryale_src_event})

# Serialization
set(euryale_serialization 				${euryale_base}/serialization)
set(euryale_serialization_serializationfactory 		${euryale_serialization}/serialization_factory)
AUX_SOURCE_DIRECTORY(${euryale_serialization} 		euryale_src_serialization)
AUX_SOURCE_DIRECTORY(${euryale_serialization_serializationfactory} euryale_src_serialization_serializationfactory)
set(euryale_package_serialization ${euryale_src_serialization} ${euryale_src_serialization_serializationfactory})
# End Serialization

# euryale::net
set(euryale_net ${euryale_base}/net)
AUX_SOURCE_DIRECTORY(${euryale_net} 			euryale_src_net)
AUX_SOURCE_DIRECTORY(${euryale_net}/endpoint    	euryale_src_net_endpoint)
AUX_SOURCE_DIRECTORY(${euryale_net}/endpoint/exception  euryale_src_net_endpoint_exception)
AUX_SOURCE_DIRECTORY(${euryale_net}/packet 		euryale_src_net_packet)
AUX_SOURCE_DIRECTORY(${euryale_base}/net/exception 	euryale_src_net_exception)
AUX_SOURCE_DIRECTORY(${euryale_base}/net/group 		euryale_src_net_group)
AUX_SOURCE_DIRECTORY(${euryale_base}/net/group/hades 	euryale_src_net_group_hades)
AUX_SOURCE_DIRECTORY(${euryale_base}/net/group/perseus 	euryale_src_net_group_perseus)

set(euryale_package_net
${euryale_src_net_endpoint}
${euryale_src_net_endpoint_exception}
${euryale_src_net_packet}
${euryale_src_net_exception}
${euryale_src_net_group}
${euryale_src_net_group_hades}
${euryale_src_net_group_perseus}
${euryale_src_net}
)
# end euryale::net

# euryale::runtime
set(euryale_runtime 					${euryale_base}/runtime)
AUX_SOURCE_DIRECTORY(${euryale_runtime} 		euryale_src_runtime)
set(euryale_package_runtime ${euryale_src_runtime})


# euryale:: qos
set(euryale_qos 					${euryale_base}/qos)
set(euryale_medusa	 				${srcEuryaleBaseDir}/qos/medusa)
AUX_SOURCE_DIRECTORY(${euryale_qos} 			euryale_src_qos)
AUX_SOURCE_DIRECTORY(${euryale_qos}/exception 		euryale_src_qos_exception)
AUX_SOURCE_DIRECTORY(${euryale_qos}/cpu_qos 		euryale_src_qos_cpuqos)
AUX_SOURCE_DIRECTORY(${euryale_qos}/mem_qos 		euryale_src_qos_memqos)
AUX_SOURCE_DIRECTORY(${euryale_qos}/io_qos 		euryale_src_qos_ioqos)
AUX_SOURCE_DIRECTORY(${euryale_qos}/net 		euryale_src_qos_net)
AUX_SOURCE_DIRECTORY(${euryale_qos}/net/packet 		euryale_src_qos_net_packet)

AUX_SOURCE_DIRECTORY(${euryale_qos}/medusa 		euryale_src_qos_medusa)
AUX_SOURCE_DIRECTORY(${euryale_qos}/medusa/exception 	euryale_src_qos_medusa_exception)
AUX_SOURCE_DIRECTORY(${euryale_qos}/medusa/cpu 		euryale_src_qos_medusa_cpu)
AUX_SOURCE_DIRECTORY(${euryale_qos}/medusa/net 		euryale_src_qos_medusa_net)
AUX_SOURCE_DIRECTORY(${euryale_qos}/medusa/mem 		euryale_src_qos_medusa_mem)
AUX_SOURCE_DIRECTORY(${euryale_qos}/medusa/io 		euryale_src_qos_medusa_io)
set(
euryale_package_qos
${euryale_src_qos}
${euryale_src_qos_exception}
${euryale_src_qos_cpuqos}
${euryale_src_qos_memqos}
${euryale_src_qos_ioqos}
${euryale_src_qos_net}
${euryale_src_qos_net_packet}
)

set(
euryale_package_qos_medusa 
${euryale_src_qos_medusa} 
${euryale_src_qos_medusa_exception} 
${euryale_src_qos_medusa_cpu} 
${euryale_src_qos_medusa_net} 
${euryale_src_qos_medusa_mem}
${euryale_src_qos_medusa_io})

MESSAGE(STATUS, "######## DEBUG: "${euryale_qos}" ########")

# Final Euryale package assembly
set(euryale_package 
${euryale_package_common}
${euryale_package_timer}
${euryale_package_threading}
${euryale_package_event}
${euryale_package_serialization}
${euryale_package_net}
${euryale_package_runtime}
${euryale_package_qos}
${euryale_package_tools}
)

set(euryale_include_dir src/org/cracs)
#END Euryale
#MESSAGE(STATUS, "######## ===>: "${euryale}" ########")

# Includes
set(srcProjectBaseDir src/org/cracs)
include_directories(${srcProjectBaseDir} ${ace_include_dir} ${crypto_include_dir})


# Shared Stheno lib
IF(BUILD_SHARED STREQUAL "TRUE")
#EURYALE	
	add_library(euryale SHARED  ${euryale_package})		
	TARGET_LINK_LIBRARIES(euryale ${ace_shared_lib} ${crypto_shared_lib} pthread)        
	SET_TARGET_PROPERTIES(euryale PROPERTIES CLEAN_DIRECT_OUTPUT 1)

        add_executable(medusad ${euryale_package_qos_medusa})
        TARGET_LINK_LIBRARIES(medusad euryale ${ace_shared_lib} ${crypto_static_lib} pthread)

        add_executable(uuid_generator ${euryale_src_common_uuid_generator})
        TARGET_LINK_LIBRARIES(uuid_generator euryale ${ace_shared_lib} ${crypto_static_lib} pthread)

        add_executable(load ${euryale_src_tools_loader_load})
        TARGET_LINK_LIBRARIES(load euryale ${ace_shared_lib} ${crypto_static_lib} pthread)

#END EURYALE
ENDIF(BUILD_SHARED STREQUAL "TRUE")

# Static STHENO lib
IF(BUILD_STATIC STREQUAL "TRUE")
#EURYALE
	add_library(euryale-static STATIC  ${euryale_package})
	TARGET_LINK_LIBRARIES(euryale-static ace_static_lib crypto_static_lib -pthread /usr/lib/librt.a /usr/lib/libdl.a)
        
        add_executable(medusad ${euryale_package_qos_medusa})
        TARGET_LINK_LIBRARIES(medusad euryale-static)
	SET_TARGET_PROPERTIES(euryale-static PROPERTIES OUTPUT_NAME "Euryale")
	SET_TARGET_PROPERTIES(euryale-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#END EURYALE	
ENDIF(BUILD_STATIC STREQUAL "TRUE")

#Tests
ADD_SUBDIRECTORY( tests )









PROJECT(stheno)
cmake_minimum_required(VERSION 2.8)
INCLUDE(../config.cmake)

#MESSAGE(STATUS, "######## stheno project ########")
set(euryale_include_dir ../euryale/src/org/cracs)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	#ADD_DEFINITIONS(-g)
	SET(LIBRARY_OUTPUT_PATH ${stheno_BINARY_DIR}/Debug)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Debug)
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
	ADD_DEFINITIONS(-O3)
	SET(LIBRARY_OUTPUT_PATH ${stheno_BINARY_DIR}/Release)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Release)
ELSE(CMAKE_BUILD_TYPE STREQUAL "Release")
#	ADD_DEFINITIONS(-g)
	SET(LIBRARY_OUTPUT_PATH ${stheno_BINARY_DIR}/Debug)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Debug)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

#Stheno
#Base settings
set(stheno_base src/org/cracs/stheno)
AUX_SOURCE_DIRECTORY(${stheno_base} stheno_src_base)

#COMMON
AUX_SOURCE_DIRECTORY(${stheno_base}/common stheno_src_common)
AUX_SOURCE_DIRECTORY(${stheno_base}/common/FT stheno_src_common_ft)
AUX_SOURCE_DIRECTORY(${stheno_base}/common/uuid stheno_src_common_uuid)
set(stheno_package_common
${stheno_src_common}
${stheno_src_common_ft}
${stheno_src_common_uuid}
)
#End Common

#EXCEPTION
AUX_SOURCE_DIRECTORY(${stheno_base}/exception stheno_src_exception)
set(stheno_package_exception ${stheno_src_exception})
#End EXCEPTION

#CORE
AUX_SOURCE_DIRECTORY(${stheno_base}/core stheno_src_core)

#P2P
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p stheno_src_core_p2p)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/routing stheno_src_core_p2p_routing)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/detection stheno_src_core_p2p_detection)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/ft stheno_src_core_p2p_ft)

AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/net/dgram stheno_src_core_p2p_net_dgram)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/net/dgram/mcast stheno_src_core_p2p_net_dgram_mcast)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/net/stream stheno_src_core_p2p_net_stream)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/net stheno_src_core_p2p_net)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/net/packet stheno_src_core_p2p_net_packet)

AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/network                   stheno_src_core_p2p_network)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/network/net               stheno_src_core_p2p_network_net)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/network/net/stream        stheno_src_core_p2p_network_net_stream)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/network/net/stream/packet stheno_src_core_p2p_network_net_stream_packet)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/network/net/dgram         stheno_src_core_p2p_network_net_dgram)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/network/net/dgram/packet  stheno_src_core_p2p_network_net_dgram_packet)



AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/discovery stheno_src_core_p2p_discovery)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/discovery/net stheno_src_core_p2p_discovery_net)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/discovery/net/packet stheno_src_core_p2p_discovery_net_packet)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/discovery/exception stheno_src_core_p2p_discovery_exception)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/discovery/peer_provider stheno_src_core_p2p_discovery_peer_provider)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/discovery/general_provider stheno_src_core_p2p_discovery_general_provider)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/discovery/service_provider stheno_src_core_p2p_discovery_service_provider)

AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/mesh stheno_src_core_p2p_mesh)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/mesh/exception stheno_src_core_p2p_mesh_exception)

AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/common stheno_src_core_p2p_common)
AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/common/exception stheno_src_core_p2p_common_exception)

AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/exception stheno_src_core_p2p_exception)

#NEW - PLUGIN SDK
#AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/sdk             stheno_src_sdk)
#AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/sdk/detection   stheno_src_sdk_detection)
#AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/sdk/discovery   stheno_src_sdk_discovery)
#AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/sdk/mesh        stheno_src_sdk_mesh)
#AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/sdk/ft          stheno_src_sdk_ft)
#AUX_SOURCE_DIRECTORY(${stheno_base}/core/p2p/sdk/net         stheno_src_sdk_net)
#set(stheno_package_sdk
#${stheno_src_sdk}
#${stheno_src_sdk_detection}
#${stheno_src_sdk_discovery}
#${stheno_src_sdk_mesh}
#${stheno_src_sdk_ft}
#${stheno_src_sdk_net}
#)

set(stheno_package_core_p2p
${stheno_src_core_p2p}
#Low level networking
${stheno_src_core_p2p_net}
${stheno_src_core_p2p_net_packet}
${stheno_src_core_p2p_net_dgram}
${stheno_src_core_p2p_net_dgram_mcast}
${stheno_src_core_p2p_net_stream}
#?
${stheno_src_core_p2p_routing}
#Network
${stheno_src_core_p2p_network}
${stheno_src_core_p2p_network_net}
${stheno_src_core_p2p_network_net_stream}
${stheno_src_core_p2p_network_net_stream_packet}
${stheno_src_core_p2p_network_net_dgram}
${stheno_src_core_p2p_network_net_dgram_packet}
#Fault-detaction
${stheno_src_core_p2p_detection}
#FT
${stheno_src_core_p2p_ft}

#Discovery
${stheno_src_core_p2p_discovery}
${stheno_src_core_p2p_discovery_net}
${stheno_src_core_p2p_discovery_net_packet}
${stheno_src_core_p2p_discovery_exception}
${stheno_src_core_p2p_discovery_general_provider}
${stheno_src_core_p2p_discovery_peer_provider}
${stheno_src_core_p2p_discovery_service_provider}
#Mesh
${stheno_src_core_p2p_mesh}
${stheno_src_core_p2p_mesh_exception}
#Common
${stheno_src_core_p2p_common}
${stheno_src_core_p2p_common_exception}
${stheno_src_core_p2p_exception}
#${stheno_package_sdk}
)
#End P2P
set(stheno_package_core
${stheno_package_core_p2p}
${stheno_src_core})
#End CORE


#SERVICES
AUX_SOURCE_DIRECTORY(${stheno_base}/service stheno_src_service)
AUX_SOURCE_DIRECTORY(${stheno_base}/service/exception stheno_src_service_exception)
set(stheno_package_services 
${stheno_src_service}
${stheno_src_service_exception}
)
#End SERVICES

set(stheno_package 
${stheno_src_base}
${stheno_package_common} 
${stheno_package_exception}
${stheno_package_core} 
#${stheno_package_p2p}
${stheno_package_services}
)
#MESSAGE(STATUS, "######## stheno sources: "${stheno_package}" ########")

set(stheno_include_dir src/org/cracs/)


# Includes
#set(srcProjectBase ${stheno_base})
include_directories(${stheno_include_dir} ${ace_include_dir} ${crypto_include_dir} ${euryale_include_dir})

# Shared Stheno lib
IF(BUILD_SHARED STREQUAL "TRUE")
	MESSAGE(STATUS, "Stheno: building shared libraries")
	add_library(stheno SHARED ${stheno_package})	
	add_dependencies(stheno euryale)
	TARGET_LINK_LIBRARIES(stheno euryale ${ace_shared_lib} ${crypto_shared_lib} pthread config++)
	SET_TARGET_PROPERTIES(stheno PROPERTIES OUTPUT_NAME "stheno")
	SET_TARGET_PROPERTIES(stheno PROPERTIES CLEAN_DIRECT_OUTPUT 1)
ENDIF(BUILD_SHARED STREQUAL "TRUE")

# Static STHENO lib
IF(BUILD_STATIC STREQUAL "TRUE")
	MESSAGE(STATUS, "Stheno: building static libraries")
	add_library(Stheno STATIC  ${sthenoPackage})
	TARGET_LINK_LIBRARIES(stheno-static euryale-static ${ace_static_lib} ${crypto_static_lib} config++)
	SET_TARGET_PROPERTIES(stheno-static PROPERTIES OUTPUT_NAME "stheno-static")
	SET_TARGET_PROPERTIES(stheno-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)	
ENDIF(BUILD_STATIC STREQUAL "TRUE")

ADD_SUBDIRECTORY( tests )

PROJECT(p3)
cmake_minimum_required(VERSION 2.8)
INCLUDE(../config.cmake)

set(euryale_include_dir ../euryale/src/org/cracs)
set(stheno_include_dir ../stheno/src/org/cracs)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	#ADD_DEFINITIONS(-g)
	SET(LIBRARY_OUTPUT_PATH ${p3_BINARY_DIR}/Debug)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Debug)
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
	ADD_DEFINITIONS(-O3)
	SET(LIBRARY_OUTPUT_PATH ${p3_BINARY_DIR}/Release)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Release)
ELSE(CMAKE_BUILD_TYPE STREQUAL "Release")
#	ADD_DEFINITIONS(-g)
	SET(LIBRARY_OUTPUT_PATH ${p3_BINARY_DIR}/Debug)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Debug)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

set(p3_base src/org/cracs/stheno/core/p2p)
set(service_base src/org/cracs/stheno/services)

#P3 BASE
AUX_SOURCE_DIRECTORY(${p3_base}/p3 p3_src)

#DETECTION
AUX_SOURCE_DIRECTORY(${p3_base}/p3/detection p3_src_detection)

#FT
AUX_SOURCE_DIRECTORY(${p3_base}/p3/ft p3_src_ft)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/ft/net p3_src_ft_net)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/ft/net/control p3_src_ft_net_control)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/ft/net/control/packet p3_src_ft_net_control_packet)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/ft/net/data p3_src_ft_net_data)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/ft/net/data/packet p3_src_ft_net_data_packet)

#DISCOVERY
AUX_SOURCE_DIRECTORY(${p3_base}/p3/discovery p3_src_discovery)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/discovery/net p3_src_discovery_net)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/discovery/net/packet p3_src_discovery_net_packet)
#P3GeneralProvider
AUX_SOURCE_DIRECTORY(${p3_base}/p3/discovery/general_provider p3_src_discovery_general_provider)

#MESH
AUX_SOURCE_DIRECTORY(${p3_base}/p3/mesh p3_src_mesh)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/mesh/net p3_src_mesh_net)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/mesh/net/packet p3_src_mesh_net_packet)

#NETWORK
AUX_SOURCE_DIRECTORY(${p3_base}/p3/network p3_src_network)

#SUPERPEER
#AUX_SOURCE_DIRECTORY(${p3_base}/p3/superpeer p3_src_superpeer)
set(p3_src_superpeer ${p3_base}/p3/superpeer/SuperPeer.cpp)


AUX_SOURCE_DIRECTORY(${p3_base}/p3/common p3_src_common)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/common/faults p3_src_common_faults)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/common/pmap p3_src_common_pmap)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/superpeer/discovery p3_src_superpeer_discovery)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/superpeer/cell p3_src_cell)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/superpeer/cell/exception p3_src_cell_exception)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/superpeer/cell/net p3_src_cell_net)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/superpeer/cell/net/packet p3_src_cell_net_packet)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/superpeer/cell/discovery p3_src_superpeer_cell_discovery)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/superpeer/cell/discovery/net p3_src_superpeer_cell_discovery_net)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/superpeer/cell/discovery/net/packet p3_src_superpeer_cell_discovery_net_packet)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/superpeer/cell/discovery/exception p3_src_superpeer_cell_discovery_exception)

#AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf p3_src_leaf)
set(p3_src_leaf ${p3_base}/p3/leaf/LeafPeer.cpp)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf/discovery p3_src_leaf_discovery)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf/discovery/net p3_src_leaf_discovery_net)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf/discovery/net_packet p3_src_leaf_discovery_net_packet)

AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf/mesh p3_src_leaf_mesh)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf/mesh/net p3_src_leaf_mesh_net)

AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf/stream p3_src_leaf_stream)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf/stream/net p3_src_leaf_stream_net)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf/stream/net/packet p3_src_leaf_stream_net_packet)
set(stream_server ${p3_src_leaf_stream} ${p3_src_leaf_stream_net} ${p3_src_leaf_stream_net_packet})

AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf/actuator p3_src_leaf_actuator)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf/actuator/net p3_src_leaf_actuator_net)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/leaf/actuator/net/packet p3_src_leaf_actuator_net_packet)
set(actuator_server ${p3_src_leaf_actuator} ${p3_src_leaf_actuator_net} ${p3_src_leaf_actuator_net_packet})


AUX_SOURCE_DIRECTORY(${p3_base}/p3/monitor p3_src_monitor)
AUX_SOURCE_DIRECTORY(${p3_base}/p3/sensor p3_src_sensor)

#Services
# - Stream
AUX_SOURCE_DIRECTORY(${service_base}/stream service_stream)
AUX_SOURCE_DIRECTORY(${service_base}/stream/net service_stream_net)
AUX_SOURCE_DIRECTORY(${service_base}/stream/net/packet service_stream_net_packet)
AUX_SOURCE_DIRECTORY(${service_base}/stream/audio service_stream_audio)
AUX_SOURCE_DIRECTORY(${service_base}/stream/video service_stream_video)
AUX_SOURCE_DIRECTORY(${service_base}/stream/event service_stream_event)
# - RPC
AUX_SOURCE_DIRECTORY(${service_base}/rpc service_rpc)
AUX_SOURCE_DIRECTORY(${service_base}/rpc/exception service_rpc_exception)
AUX_SOURCE_DIRECTORY(${service_base}/rpc/net service_rpc_net)
AUX_SOURCE_DIRECTORY(${service_base}/rpc/net/packet service_rpc_net_packet)
# - Actuator
AUX_SOURCE_DIRECTORY(${service_base}/actuator service_actuator)
AUX_SOURCE_DIRECTORY(${service_base}/actuator/exception service_actuator_exception)
AUX_SOURCE_DIRECTORY(${service_base}/actuator/net service_actuator_net)
AUX_SOURCE_DIRECTORY(${service_base}/actuator/net/packet service_actuator_net_packet)


set(p3_package
#COMMON
${p3_src}
${p3_src_common}
${p3_src_common_faults}
${p3_src_common_pmap}
${p3_src_detection}
#FT
${p3_src_ft}
${p3_src_ft_net}
${p3_src_ft_net_control}
${p3_src_ft_net_control_packet}
${p3_src_ft_net_data}
${p3_src_ft_net_data_packet}
#DISCOVERY
${p3_src_discovery}
${p3_src_discovery_net}
${p3_src_discovery_net_packet}
${p3_src_discovery_general_provider}
#MESH
${p3_src_mesh}
${p3_src_mesh_net}
${p3_src_mesh_net_packet}
#NETWORK
${p3_src_network}
#SUPERPEER
${p3_src_superpeer}
${p3_src_superpeer_discovery}
${p3_src_cell}
${p3_src_cell_exception}
${p3_src_cell_net}
${p3_src_cell_net_packet}
${p3_src_superpeer_cell_discovery}
${p3_src_superpeer_cell_discovery_net}
${p3_src_superpeer_cell_discovery_net_packet}
${p3_src_superpeer_cell_discovery_exception}
#LEAF
${p3_src_leaf}
${p3_src_leaf_discovery}
${p3_src_leaf_discovery_net}
${p3_src_leaf_discovery_net_packet}
${p3_src_leaf_mesh}
${p3_src_leaf_mesh_net}
${p3_src_leaf_stream}
${p3_src_leaf_stream_net}
${p3_src_leaf_stream_net_packet}
${p3_src_leaf_actuator}
${p3_src_leaf_actuator_net}
${p3_src_leaf_actuator_net_packet}

#MONITOR
#${p3_src_monitor}
#SENSOR
#${p3_src_sensor}
#static services
${service_stream}
${service_stream_net}
${service_stream_net_packet}
${service_stream_audio}
#${service_stream_video}
#${service_stream_event}
#rpc
${service_rpc}
${service_rpc_exception}
${service_rpc_net}
${service_rpc_net_packet}
#actuator
${service_actuator}
${service_actuator_exception}
${service_actuator_net}
${service_actuator_net_packet}

)

#MESSAGE(STATUS, "######## p3: "${p3_src_ft_net_control_packet}" ########")

set(p3_superpeer ${p3_base}/p3/superpeer/superpeer_main)
set(p3_superpeer_plugin_file ${p3_base}/p3/superpeer/superpeer_plugin_startup)

set(p3_leaf ${p3_base}/p3/leaf/leaf_main)
set(p3_leaf_plugin_file ${p3_base}/p3/leaf/leaf_plugin_startup)

set(p3_monitor ${p3_base}/p3/monitor/monitor_main)
set(p3_monitor_plugin_file ${p3_base}/p3/monitor/monitor_plugin_startup)

set(p3_sensor ${p3_base}/p3/sensor/sensor_main)
set(p3_sensor_plugin_file ${p3_base}/p3/sensor/sensor_plugin_startup)

set(p3_include_dir src/org/cracs/)


# Includes
include_directories(${p3_include_dir} ${stheno_include_dir} ${ace_include_dir} ${crypto_include_dir} ${euryale_include_dir})

IF(BUILD_SHARED STREQUAL "TRUE")
	MESSAGE(STATUS, "P3: building shared libraries")
	#add_library(p3 SHARED ${p3_package})
	#add_dependencies(p3 stheno)
	#TARGET_LINK_LIBRARIES(p3 stheno euryale ${ace_shared_lib} ${crypto_shared_lib} pthread config++)	       

	add_library(p3 SHARED ${p3_package})
        TARGET_LINK_LIBRARIES(p3 stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread)

        add_library(superpeer_plugin SHARED ${p3_superpeer_plugin_file})
        TARGET_LINK_LIBRARIES(superpeer_plugin p3 stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread)
        add_executable(superpeer ${p3_superpeer} 
                ${service_stream} 
                ${service_stream_net} 
                ${service_stream_net_packet} 
                ${service_stream_audio} 
                ${stream_server}                 
                ${service_actuator}
                ${service_actuator_exception}
                ${service_actuator_net} 
                ${service_actuator_net_packet} 
                ${actuator_server} 
                ${p3_src}
                ${service_rpc}
                ${service_rpc_exception}
                ${service_rpc_net}
                ${service_rpc_net_packet}
                )
        TARGET_LINK_LIBRARIES(superpeer stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread)

        add_library(leaf_plugin SHARED ${p3_leaf_plugin_file})
        TARGET_LINK_LIBRARIES(leaf_plugin p3 stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread)
        add_executable(leaf ${p3_leaf})
        TARGET_LINK_LIBRARIES(leaf stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread)

	#SENSOR
        add_library(sensor_plugin SHARED ${p3_sensor_plugin_file})
        TARGET_LINK_LIBRARIES(sensor_plugin p3 stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread)
        add_executable(sensor ${p3_sensor})
        TARGET_LINK_LIBRARIES(sensor stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread)

	#MONITOR
        add_library(monitor_plugin SHARED ${p3_monitor_plugin_file})
        TARGET_LINK_LIBRARIES(monitor_plugin p3 stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread)
        add_executable(monitor ${p3_monitor} ${p3_base}/p3/monitor/RPCTest.cpp ${p3_base}/p3/monitor/MeshTest.cpp ${p3_base}/p3/monitor/ActuatorTest.cpp ${p3_base}/p3/monitor/StreamTest.cpp
            ${p3_base}/p3/monitor/MultipleRPCTest.cpp ${p3_base}/p3/monitor/ClientMultipleRPCTest.cpp)
        TARGET_LINK_LIBRARIES(monitor p3 stheno euryale ${ace_shared_lib} ${crypto_static_lib} pthread)

        #SET_TARGET_PROPERTIES(stheno PROPERTIES CLEAN_DIRECT_OUTPUT 1)
ENDIF(BUILD_SHARED STREQUAL "TRUE")

ADD_SUBDIRECTORY( tests )


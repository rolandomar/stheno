cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS, "######## CMAKE_ONCE: "${CMAKE_ONCE} " ########")

option(USE_CLANG "build application with clang" ON) # OFF is the default

IF(NOT DEFINED(CMAKE_ONCE))
IF(NOT CMAKE_ONCE STREQUAL "TRUE")

set(CMAKE_ONCE "TRUE")
enable_testing()


#INCLUDE(CMakePrintSystemInformation)
SET(CMAKE_VERBOSE_MAKEFILE ON)

# Config
SET(BUILD_SHARED "TRUE")
SET(BUILD_STATIC "FALSE")
SET(BUILD_RPM    "FALSE")
# End Config
MESSAGE(STATUS, "######## Building type:->"${CMAKE_BUILD_TYPE}"<-########")
#IF(NOT DEFINED(CMAKE_BUILD_TYPE))
#set(CMAKE_BUILD_TYPE "Debug")
#ENDIF(NOT DEFINED(CMAKE_BUILD_TYPE))
IF(CMAKE_BUILD_TYPE STREQUAL "")
set(CMAKE_BUILD_TYPE "Debug")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "")

IF(USE_CLANG)
  MESSAGE(STATUS, "######## clang Toolchain:->"${USE_CLANG}"<-########")
  SET(CMAKE_TOOLCHAIN_PREFIX "llvm-")  
  SET(CMAKE_C_COMPILER                   "/usr/bin/clang")
  SET(CMAKE_CXX_COMPILER                 "/usr/bin/clang++")
  SET (CMAKE_C_FLAGS_INIT                "-Wall -std=c99")
  SET (CMAKE_C_FLAGS_DEBUG_INIT          "-g")
  SET (CMAKE_C_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
  SET (CMAKE_C_FLAGS_RELEASE_INIT        "-O4 -DNDEBUG")
  SET (CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")
  SET(CMAKE_C_FLAGS                      "${CMAKE_C_FLAGS} -fmessage-length=0 ")

  
  SET (CMAKE_CXX_FLAGS_INIT                "-Wall")
  SET (CMAKE_CXX_FLAGS_DEBUG_INIT          "-g")
  SET (CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
  SET (CMAKE_CXX_FLAGS_RELEASE_INIT        "-O4 -DNDEBUG")
  SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")
  SET (CMAKE_CCC_FLAGS                     "${CMAKE_C_FLAGS} -fmessage-length=0 ")
ENDIF(USE_CLANG)


IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
IF(CMAKE_COMPILER_IS_GNUCC)
MESSAGE(STATUS, "######## GCC Toolchain:->ON<-########")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0 -ggdb")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0 -ggdb")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ELSE(CMAKE_BUILD_TYPE STREQUAL "Debug")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")



  


#FIND_PROGRAM(CCACHE_BIN "ccache")
#IF(CCACHE_BIN STREQUAL "CCACHE_BIN-NOTFOUND")
#    SET(HAS_CCACHE false)
#    MESSAGE("Executable not found: ccache")    
#ELSE(CCACHE_BIN STREQUAL "CCACHE_BIN-NOTFOUND")
#    SET(HAS_CCACHE true)   
#    MESSAGE("Executable found: ccache - ${CCACHE_BIN}")
#ENDIF(CCACHE_BIN STREQUAL "CCACHE_BIN-NOTFOUND")

#IF(HAS_CCACHE)  
#    SET(CMAKE_CXX_COMPILER_ARG1 "/usr/bin/g++")
#    SET(CMAKE_CXX_COMPILER "/usr/bin/ccache")    
#ENDIF(HAS_CCACHE)  


MESSAGE(STATUS, "######## Building type: "${CMAKE_BUILD_TYPE}" ########")



#IF(WIN32)
#MESSAGE(STATUS,"Win32 not yet supported!")
#ELSE(WIN32)
#ENDIF(WIN32)

set(ace_include_dir /usr/local/include/ace)


IF(WIN32)
	set(ace_home_dir 			E:/ACE_wrappers)
	set(ace_include_dir 			${ace_home_dir})
	set(ace_shared_lib 			${ace_home_dir}/lib/ACEd.dll)
	set(crypto_home_dir 			E:/OpenSSL)
	set(crypto_include_dir 			${crypto_home_dir}/include)
	set(crypto_shared_lib 			${crypto_home_dir}/lib/VC/libeay32MD.lib)
ELSE(WIN32)
	set(ace_include_dir 			/usr/local/include/ace)
	set(ace_shared_lib 			ACE)
	set(crypto_include_dir 			"/usr/include")
	set(crypto_shared_lib 			crypto)
	#STATIC libs

        add_library(ace_static_lib STATIC IMPORTED)
        set_property(TARGET ace_static_lib PROPERTY IMPORTED_LOCATION /usr/local/lib/libACE.a)
        add_library(crypto_static_lib STATIC IMPORTED)
        set_property(TARGET crypto_static_lib PROPERTY IMPORTED_LOCATION /usr/lib/libcrypto.a)
        add_library(pthread_static_lib STATIC IMPORTED)
        set_property(TARGET pthread_static_lib PROPERTY IMPORTED_LOCATION /usr/lib/libpthread.a)

ENDIF(WIN32)

MESSAGE(STATUS,${aceSharedLib})
MESSAGE(STATUS,${aceIncludeDir})
MESSAGE(STATUS,${cryptoIncludeDir})
MESSAGE(STATUS,"pthread static")
MESSAGE(STATUS,${pthreadStaticLib})

IF (UNIX)
  ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
  
  ADD_CUSTOM_COMMAND(
    COMMENT "distribution clean"
    COMMAND bin/clean.sh
    TARGET  distclean
  )
ENDIF(UNIX)

ENDIF(NOT CMAKE_ONCE STREQUAL "TRUE")
ENDIF(NOT DEFINED(CMAKE_ONCE))




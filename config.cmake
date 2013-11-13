cmake_minimum_required(VERSION 2.8)
#MESSAGE(STATUS, "######## CMAKE_ONCE: "${CMAKE_ONCE} " ########")

option(USE_CLANG "build application with clang" ON) # OFF is the default

IF(NOT DEFINED(CMAKE_ONCE))
IF(NOT CMAKE_ONCE STREQUAL "TRUE")

set(CMAKE_ONCE "TRUE")

# Config
SET(BUILD_SHARED "TRUE")
SET(BUILD_STATIC "FALSE")
SET(BUILD_RPM    "FALSE")
# End Config

IF(NOT DEFINED CMAKE_BUILD_TYPE)
MESSAGE(STATUS,"Stheno: debugging mode")
set(CMAKE_BUILD_TYPE Debug CACHE STR "Debug mode" FORCE)
ENDIF(NOT DEFINED CMAKE_BUILD_TYPE)

IF(CMAKE_BUILD_TYPE STREQUAL "")
MESSAGE(STATUS, "Stheno: debugging mode")
set(CMAKE_BUILD_TYPE Debug CACHE STR "Debug mode" FORCE)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "")

MESSAGE(STATUS, "Stheno: Building type: ${CMAKE_BUILD_TYPE}")

SET(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE TRUE CACHE BOOL "Verbose output" FORCE)

IF(USE_CLANG)
  MESSAGE(STATUS, "Stheno: using clang toolchain")
  SET(CMAKE_TOOLCHAIN_PREFIX "llvm-")  
  SET(CMAKE_C_COMPILER   "/usr/bin/clang")
  SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
ELSE(USE_CLANG)
  MESSAGE(STATUS, "Stheno: using gcc toolchain")
ENDIF(USE_CLANG)

SET(CMAKE_VERBOSE_MAKEFILE ON)
IF(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  SET(CMAKE_C_FLAGS "-O4")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")

  SET(CMAKE_CXX_FLAGS_RELEASE "-O4")
  SET(CMAKE_CCC_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0 ")
ELSE(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  SET(CMAKE_C_FLAGS_DEBUG "-g3 -gdwarf-2")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")

  SET(CMAKE_CXX_FLAGS_DEBUG "-g3 -gdwarf-2")
  SET(CMAKE_CCC_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0 ")
ENDIF(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")

set(ace_include_dir /usr/local/include/ace)

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

# MESSAGE(STATUS,${aceSharedLib})
# MESSAGE(STATUS,${aceIncludeDir})
# MESSAGE(STATUS,${cryptoIncludeDir})
# MESSAGE(STATUS,"pthread static")
# MESSAGE(STATUS,${pthreadStaticLib})

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


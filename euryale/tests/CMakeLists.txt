#ADD_SUBDIRECTORY( hades )
#ADD_SUBDIRECTORY( ByteBuffer )
include_directories(${srcProjectBaseDir} ${aceIncludeDir} ${cryptoIncludeDir})
ADD_EXECUTABLE(hades_realm hades_realm)
TARGET_LINK_LIBRARIES(hades_realm euryale ${aceSharedLib} ${cryptoSharedLib} pthread)
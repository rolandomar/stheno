include_directories(${srcProjectBaseDir} ${aceIncludeDir} ${cryptoIncludeDir})
ADD_EXECUTABLE(ByteBuffer_test ByteBuffer_test)
TARGET_LINK_LIBRARIES(ByteBuffer_test Euryale ${aceSharedLib} ${cryptoSharedLib} pthread)



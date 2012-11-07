include_directories(${srcProjectBaseDir} ${aceIncludeDir} ${cryptoIncludeDir})
ADD_EXECUTABLE(hades_test hades_test)
TARGET_LINK_LIBRARIES(hades_test Euryale ${aceSharedLib} ${cryptoSharedLib} pthread)

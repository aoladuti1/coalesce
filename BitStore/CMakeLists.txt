file(GLOB SOURCES src/*.cpp)
add_library(BitStore ${STATIC_OR_SHARED} ${SOURCES})
target_include_directories(BitStore PUBLIC "${CoalesceProj_SOURCE_DIR}/BitStore/inc")

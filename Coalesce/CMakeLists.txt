file(GLOB SOURCES src/*.cpp)
add_executable(Coalesce ${SOURCES})
# Following line not necessary as dependency BitStore has a PUBLIC include command
# target_include_directories(Coalesce PRIVATE "${CoalesceProj_SOURCE_DIR}/BitStore/inc")
target_link_directories(Coalesce PUBLIC  "${CoalesceProj_SOURCE_DIR}/Coalesce/lib")
target_link_libraries(Coalesce "BitStore")
target_include_directories(Coalesce PRIVATE "${CoalesceProj_SOURCE_DIR}/Coalesce/inc")


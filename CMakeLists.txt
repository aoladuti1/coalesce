cmake_minimum_required(VERSION 3.15.0)
project(CoalesceProj VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
include(CPack)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CoalesceProj_SOURCE_DIR}/Coalesce/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CoalesceProj_SOURCE_DIR}/Coalesce/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CoalesceProj_SOURCE_DIR}/Coalesce/lib)
set(STATIC_OR_SHARED STATIC)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})


add_subdirectory(BitStore)
add_subdirectory(Coalesce)




cmake_minimum_required(VERSION 3.15.0)
project(CoalesceProj VERSION 0.1.0 LANGUAGES CXX)

include(CPack)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(STATIC_OR_SHARED STATIC)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(LINK_MODE EXTERNAL)

file(GLOB SOURCES src/*.cpp testing/*.cpp)
add_executable(coalesce ${SOURCES})

target_include_directories(coalesce PUBLIC inc)
target_include_directories(coalesce PUBLIC testing)
target_include_directories(coalesce PUBLIC include)

add_executable(csc ${SOURCES})

target_include_directories(csc PUBLIC inc)
target_include_directories(csc PUBLIC testing)
target_include_directories(csc PUBLIC include)



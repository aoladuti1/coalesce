cmake_minimum_required(VERSION 3.15.0)
project(CoalesceProj VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
include(CPack)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(STATIC_OR_SHARED STATIC)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(LINK_MODE EXTERNAL)

file(GLOB SOURCES src/*.cpp)
add_executable(Coalesce ${SOURCES})

list(APPEND CMAKE_PREFIX_PATH lib/cmake/bitstore)
find_package(BitStorePackage CONFIG REQUIRED)
target_link_libraries(Coalesce PRIVATE BS::BitStore)
target_include_directories(Coalesce PUBLIC include)
# --- Development only ---



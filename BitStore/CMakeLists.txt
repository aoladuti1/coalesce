cmake_minimum_required(VERSION 3.15.0)
project(BitStoreProj VERSION 0.1.0 LANGUAGES CXX)
file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS inc/*.hpp)

file(GLOB PUBLIC_HEADERS inc/bitstore.hpp)
set(CMK_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake/bitstore)
set(PACKAGE_NAME BitStorePackage)
set(STATIC_OR_SHARED STATIC)
set(TEST_DRIVE ${CMAKE_SOURCE_DIR}/testing/test.cpp)

# Create library file
add_library(BitStore ${STATIC_OR_SHARED} ${SOURCES} ${HEADERS})
if (EXISTS ${TEST_DRIVE})
    add_executable(Testing ${TEST_DRIVE})
    set_target_properties(Testing PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testing
    )
    target_include_directories(Testing PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
endif()

# Include inc directory for THIS project
target_include_directories(BitStore PRIVATE inc)


# Assign the output routes to TargetSet
install(TARGETS BitStore EXPORT TargetSet
        LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib
        ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib
        RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Export targets
install(EXPORT TargetSet 
        FILE ${PACKAGE_NAME}.cmake
        NAMESPACE BS::
        DESTINATION ${CMK_OUTPUT})

# Generate and install package config files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Configure the package
configure_package_config_file(
    # This file is hand-written and is needed for package initialisation
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PACKAGE_NAME}Config.cmake.in
    # This is the standard config file
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake
    INSTALL_DESTINATION ${CMK_OUTPUT}
)

# Install the configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake
    DESTINATION ${CMK_OUTPUT}
)

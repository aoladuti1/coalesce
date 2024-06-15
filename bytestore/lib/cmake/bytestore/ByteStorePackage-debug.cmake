#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "CSC::ByteStore" for configuration "Debug"
set_property(TARGET CSC::ByteStore APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CSC::ByteStore PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "C:/Repositories/coalesce/bytestore/lib/libByteStore.a"
  )

list(APPEND _cmake_import_check_targets CSC::ByteStore )
list(APPEND _cmake_import_check_files_for_CSC::ByteStore "C:/Repositories/coalesce/bytestore/lib/libByteStore.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

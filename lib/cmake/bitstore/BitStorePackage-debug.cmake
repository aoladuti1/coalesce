#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "BS::BitStore" for configuration "Debug"
set_property(TARGET BS::BitStore APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(BS::BitStore PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "C:/Repositories/coalesce/bitstore/lib/libBitStore.a"
  )

list(APPEND _cmake_import_check_targets BS::BitStore )
list(APPEND _cmake_import_check_files_for_BS::BitStore "C:/Repositories/coalesce/bitstore/lib/libBitStore.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ccqlite" for configuration "Debug"
set_property(TARGET ccqlite APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(ccqlite PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/ccqlited.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/ccqlited.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ccqlite )
list(APPEND _IMPORT_CHECK_FILES_FOR_ccqlite "${_IMPORT_PREFIX}/lib/ccqlited.lib" "${_IMPORT_PREFIX}/bin/ccqlited.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

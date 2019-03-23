# This script locates the RapidXML library
# ------------------------------------
#
# usage:
# find_package(RAPIDXML ...)
#
# searches in RAPIDXML_ROOT and usual locations
#
# Sets RAPIDXML_DIR

set (VCPKG_TARGET_TRIPLET "x86-windows")
find_path (RAPIDXML_DIR rapidxml.hpp rapidxml_iterators.hpp rapidxml_print.hpp rapidxml_utils.hpp
    PATH_SUFFIXES include
    PATHS
    "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")

if (NOT RAPIDXML_DIR)
    if (RapidXML_FIND_REQUIRED) #prefix is filename, case matters
        message (FATAL_ERROR "Could not find RapidXML!")
    elseif (NOT RapidXML_FIND_QUIETLY)
        message ("Could not find RapidXML!")
    endif (RapidXML_FIND_REQUIRED)
endif (NOT RAPIDXML_DIR)

if (NOT RapidXML_FIND_QUIETLY)
    message ("Found RapidXML: ${RAPIDXML_DIR}")
    set (rapidxml_FOUND 1)
endif (NOT RapidXML_FIND_QUIETLY)

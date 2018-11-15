set(VCPKG_TARGET_TRIPLET "x86-windows")
message(STATUS "VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
message(STATUS "_VCPKG_INSTALLED_DIR: ${_VCPKG_INSTALLED_DIR}")

if("${CMAKE_CONFIGURATION_TYPES}" MATCHES "Debug")
    include_directories(${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib/mswud ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
    set(wxWidgets_LIB_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib)
    set(wxWidgets_LIBRARIES
        ${wxWidgets_LIB_DIR}/wxbase31ud_net.lib
        ${wxWidgets_LIB_DIR}/wxmsw31ud_core.lib
        ${wxWidgets_LIB_DIR}/wxbase31ud.lib
        ${wxWidgets_LIB_DIR}/libpng16d.lib
        ${wxWidgets_LIB_DIR}/zlibd.lib
        comctl32 rpcrt4
        )
elseif("${CMAKE_CONFIGURATION_TYPES}" MATCHES "Release")
    include_directories(${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/mswu ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
    set(wxWidgets_LIB_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)
    set(wxWidgets_LIBRARIES
        ${wxWidgets_LIB_DIR}/wxbase31u_net.lib
        ${wxWidgets_LIB_DIR}/wxmsw31u_core.lib
        ${wxWidgets_LIB_DIR}/wxbase31u.lib
        ${wxWidgets_LIB_DIR}/libpng16.lib
        ${wxWidgets_LIB_DIR}/zlib.lib
        comctl32 rpcrt4
        )
endif()

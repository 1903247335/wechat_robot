# POST_BUILD: copy vcpkg MinGW DLLs next to appwx (must match linked Debug/Release).
# Single-config Ninja may pass empty CONFIG; use CMAKE_WX_BUILD_TYPE from parent.

set(_debug FALSE)
if(CONFIG STREQUAL "Debug")
    set(_debug TRUE)
endif()
if(CMAKE_WX_BUILD_TYPE MATCHES "^(Debug|RelWithDebInfo)$")
    set(_debug TRUE)
endif()

set(_rel "${TRIPLET_BIN_REL}")
set(_dbg "${TRIPLET_BIN_DBG}")
if(_debug)
    set(_primary "${_dbg}")
    set(_fallback "${_rel}")
else()
    set(_primary "${_rel}")
    set(_fallback "${_dbg}")
endif()

message(STATUS "wx_copy_vcpkg_dlls: OUT=${OUT} debug=${_debug} primary=${_primary}")

function(wx_force_copy src dst)
    if(EXISTS "${src}")
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E copy "${src}" "${dst}"
            RESULT_VARIABLE _rv)
        if(NOT _rv EQUAL 0)
            message(WARNING "wx_copy_vcpkg_dlls: copy failed ${src} -> ${dst}")
        endif()
    endif()
endfunction()

# Names that must come from the same vcpkg tree as linked libprotobuf / libabseil
set(_names libwinpthread-1.dll libabseil_dll.dll)
if(_debug)
    list(APPEND _names libprotobufd.dll libprotobuf-lited.dll)
else()
    list(APPEND _names libprotobuf.dll libprotobuf-lite.dll)
endif()
# Protobuf 33+ may pull these (copy if present)
list(APPEND _names libutf8_validity.dll libutf8_range.dll zlib1.dll)

foreach(_name IN LISTS _names)
    wx_force_copy("${_primary}/${_name}" "${OUT}/${_name}")
    if(NOT EXISTS "${OUT}/${_name}")
        wx_force_copy("${_fallback}/${_name}" "${OUT}/${_name}")
    endif()
endforeach()

# Catch-all: any other libprotobuf* / libutf8* in primary folder (optional deps)
if(EXISTS "${_primary}")
    file(GLOB _extra "${_primary}/libprotobuf*.dll" "${_primary}/libutf8*.dll")
    foreach(_dll IN LISTS _extra)
        get_filename_component(_n "${_dll}" NAME)
        wx_force_copy("${_dll}" "${OUT}/${_n}")
    endforeach()
endif()

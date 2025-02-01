#############################################################################
# Name:        build/cmake/lib/webp.cmake
# Purpose:     Use external or internal libwebp
# Author:
# Created:     2025-01-31
# Copyright:   (c) 2025 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxUSE_LIBWEBP STREQUAL "builtin")
    set(WEBP_ROOT "${wxSOURCE_DIR}/src/webp")
    set(WEBP_BUILD_ROOT "${CMAKE_CURRENT_BINARY_DIR}/webp-build")

    # static library
    set(WEBP_LINK_STATIC       ON)
    # disable tools
    set(WEBP_BUILD_ANIM_UTILS OFF)
    set(WEBP_BUILD_CWEBP      OFF)
    set(WEBP_BUILD_DWEBP      OFF)
    set(WEBP_BUILD_GIF2WEBP   OFF)
    set(WEBP_BUILD_IMG2WEBP   OFF)
    set(WEBP_BUILD_VWEBP      OFF)
    set(WEBP_BUILD_WEBPINFO   OFF)
    set(WEBP_BUILD_WEBPMUX    OFF)
    set(WEBP_BUILD_EXTRAS     OFF)
    set(WEBP_BUILD_WEBP_JS    OFF)
    set(WEBP_BUILD_FUZZTEST   OFF)

    add_subdirectory("${WEBP_ROOT}" "${WEBP_BUILD_ROOT}" EXCLUDE_FROM_ALL)

    mark_as_advanced(WEBP_BITTRACE)
    mark_as_advanced(WEBP_BUILD_LIBWEBPMUX)
    mark_as_advanced(WEBP_ENABLE_SIMD)
    mark_as_advanced(WEBP_ENABLE_SWAP_16BIT_CSP)
    mark_as_advanced(WEBP_ENABLE_WUNUSED_RESULT)
    mark_as_advanced(WEBP_LINK_STATIC)
    mark_as_advanced(WEBP_NEAR_LOSSLESS)
    mark_as_advanced(WEBP_UNICODE)
    mark_as_advanced(WEBP_USE_THREAD)

    get_property(webpTargets DIRECTORY "${WEBP_ROOT}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(webpTarget IN LISTS webpTargets)
        set_target_properties(${webpTarget} PROPERTIES FOLDER "Third Party Libraries/WebP")
    endforeach()

    set(WebP_LIBRARIES webp webpdemux)
    # set(WebP_INCLUDE_DIRS ${WEBP_ROOT}/src)

elseif(wxUSE_LIBWEBP)
    find_package(WebP REQUIRED)
endif()

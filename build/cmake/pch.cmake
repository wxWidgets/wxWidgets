#############################################################################
# Name:        build/cmake/pch.cmake
# Purpose:     precompiled header support for wxWidgets
# Author:      Maarten Bent
# Created:     2022-04-15
# Copyright:   (c) 2022 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if((wxBUILD_PRECOMP STREQUAL "ON" AND CMAKE_VERSION VERSION_LESS "3.16") OR (wxBUILD_PRECOMP STREQUAL "COTIRE"))
    if(CMAKE_GENERATOR STREQUAL "Xcode")
        # wxWidgets does not use the unity features of cotire so we can
        # include Obj-C files when using precompiled headers with Xcode
        set(COTIRE_UNITY_SOURCE_EXCLUDE_EXTENSIONS "" CACHE STRING "wxWidgets override of cotire exclude")
    endif()
    include(cotire)
endif()

# Enable precompiled headers for target
macro(wx_target_enable_precomp target_name prec_header)
    if(wxBUILD_PRECOMP)
        target_compile_definitions(${target_name} PRIVATE WX_PRECOMP)
        if(USE_COTIRE)
            set_target_properties(${target_name} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT ${prec_header})
            set_target_properties(${target_name} PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
            cotire(${target_name})
        else()
            # Only use pch when there are at least two source files
            get_target_property(cpp_source_files ${target_name} SOURCES)
            list(FILTER cpp_source_files INCLUDE REGEX ".*(\\.cpp|\\.cxx)$")
            list(LENGTH cpp_source_files cpp_source_count)
            if(cpp_source_count GREATER_EQUAL 2)
                target_precompile_headers(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${prec_header}>")
            endif()
            get_target_property(mm_source_files ${target_name} SOURCES)
            list(FILTER mm_source_files INCLUDE REGEX ".*\\.mm$")
            list(LENGTH mm_source_files mm_source_count)
            if(mm_source_count GREATER_EQUAL 2)
                target_precompile_headers(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:OBJCXX>:${prec_header}>")
            endif()
        endif()
    elseif(MSVC)
        target_compile_definitions(${target_name} PRIVATE NOPCH)
    endif()
endmacro()

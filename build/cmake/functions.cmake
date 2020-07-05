#############################################################################
# Name:        build/cmake/functions.cmake
# Purpose:     Common CMake functions for wxWidgets
# Author:      Tobias Taschner
# Created:     2016-09-20
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(CMakeDependentOption)
include(CMakeParseArguments)           # For compatibility with CMake < 3.4
include(ExternalProject)
if(CMAKE_GENERATOR STREQUAL "Xcode")
    # wxWidgets does not use the unity features of cotire so we can
    # include Obj-C files when using precompiled headers with Xcode
    set(COTIRE_UNITY_SOURCE_EXCLUDE_EXTENSIONS "" CACHE STRING "wxWidgets override of cotire exclude")
endif()
include(cotire)                        # For precompiled header handling
include(CMakePrintHelpers)

# This function adds a list of headers to a variable while prepending
# include/ to the path
macro(wx_add_headers src_var)
    set(headers)
    foreach(header ${ARGN})
        list(APPEND headers ${wxSOURCE_DIR}/include/${header})
        if(header MATCHES "\\.cpp$")
            # .cpp files in include directory should not be compiled
            if (wxBUILD_MONOLITHIC)
                # set_source_files_properties only works within the same CMakeLists.txt
                list(APPEND wxMONO_NONCOMPILED_CPP_FILES ${wxSOURCE_DIR}/include/${header})
                set(wxMONO_NONCOMPILED_CPP_FILES ${wxMONO_NONCOMPILED_CPP_FILES} PARENT_SCOPE)
            else()
                set_source_files_properties(${wxSOURCE_DIR}/include/${header}
                    PROPERTIES HEADER_FILE_ONLY TRUE)
            endif()
        endif()
    endforeach()
    list(APPEND ${src_var} ${headers})
endmacro()

# Add sources from a ..._SRC variable and headers from a ..._HDR
macro(wx_append_sources src_var source_base_name)
    if(NOT DEFINED ${src_var})
        set(${src_var} "")
    endif()
    if(DEFINED ${source_base_name}_SRC)
        wx_list_add_prefix(${src_var} "${wxSOURCE_DIR}/" ${${source_base_name}_SRC})
    endif()
    if(DEFINED ${source_base_name}_HDR)
        wx_add_headers(${src_var} ${${source_base_name}_HDR})
    endif()
endmacro()

# Add prefix to list of items
# wx_list_add_prefix(<out_var> <pref> <items...>)
macro(wx_list_add_prefix out_var prefix)
    foreach(item ${ARGN})
        list(APPEND ${out_var} ${prefix}${item})
    endforeach()
endmacro()

# Older cmake versions don't support string(APPEND ...) provide a workaround
macro(wx_string_append var str)
    set(${var} ${${var}}${str})
endmacro()

# wx_install(...)
# Forward to install call if wxBUILD_INSTALL is enabled
macro(wx_install)
    if(wxBUILD_INSTALL)
        install(${ARGN})
    endif()
endmacro()

# Set properties common to builtin third party libraries and wx libs
function(wx_set_common_target_properties target_name)
    cmake_parse_arguments(wxCOMMON_TARGET_PROPS "DEFAULT_WARNINGS" "" "" ${ARGN})

    set_target_properties(${target_name} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${wxOUTPUT_DIR}${wxPLATFORM_LIB_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${wxOUTPUT_DIR}${wxPLATFORM_LIB_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${wxOUTPUT_DIR}${wxPLATFORM_LIB_DIR}"
        )

    if(MSVC)
        if(wxCOMMON_TARGET_PROPS_DEFAULT_WARNINGS)
            set(MSVC_WARNING_LEVEL "/W3")
        else()
            set(MSVC_WARNING_LEVEL "/W4")
        endif()
        target_compile_options(${target_name} PRIVATE ${MSVC_WARNING_LEVEL})
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND NOT wxCOMMON_TARGET_PROPS_DEFAULT_WARNINGS)
        target_compile_options(${target_name} PRIVATE
            -Wall
            )
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND NOT wxCOMMON_TARGET_PROPS_DEFAULT_WARNINGS)
        target_compile_options(${target_name} PRIVATE
            -Wall
            -Wno-ignored-attributes
            )
    endif()

    if(CMAKE_USE_PTHREADS_INIT)
        target_compile_options(${target_name} PRIVATE "-pthread")
        # clang++.exe: warning: argument unused during compilation: '-pthread' [-Wunused-command-line-argument]
        if(NOT (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
            set_target_properties(${target_name} PROPERTIES LINK_FLAGS "-pthread")
        endif()
    endif()
endfunction()

# Set common properties on wx library target
function(wx_set_target_properties target_name is_base)
    if(${target_name} MATCHES "wx.*")
        string(SUBSTRING ${target_name} 2 -1 target_name_short)
    else()
        set(target_name_short ${target_name})
    endif()

    # Set library name according to:
    # docs/contributing/about-platform-toolkit-and-library-names.md
    if(is_base)
        set(lib_toolkit base)
    else()
        set(lib_toolkit ${wxBUILD_TOOLKIT}${wxBUILD_WIDGETSET})
    endif()
    if(MSVC)
        set(lib_version ${wxMAJOR_VERSION}${wxMINOR_VERSION})
    else()
        set(lib_version ${wxMAJOR_VERSION}.${wxMINOR_VERSION})
    endif()
    if(wxUSE_UNICODE)
        set(lib_unicode u)
    else()
        set(lib_unicode)
    endif()
    if(NOT target_name_short STREQUAL "base" AND NOT target_name_short STREQUAL "mono")
        # Do not append library name for base library
        set(lib_suffix _${target_name_short})
    else()
        set(lib_suffix)
    endif()
    set(lib_flavour "")
    if(wxBUILD_FLAVOUR)
        set(lib_flavour "_${wxBUILD_FLAVOUR}")
        string(REPLACE "-" "_" lib_flavour ${lib_flavour})
    endif()

    if(WIN32)
        if(MSVC)
            # match visual studio name
            set_target_properties(${target_name}
                PROPERTIES
                    OUTPUT_NAME "wx${lib_toolkit}${lib_version}${lib_unicode}${lib_flavour}${lib_suffix}"
                    OUTPUT_NAME_DEBUG "wx${lib_toolkit}${lib_version}${lib_unicode}d${lib_flavour}${lib_suffix}"
                    PREFIX ""
                )
        else()
            # match configure name (mingw, cygwin)
            set_target_properties(${target_name}
                PROPERTIES
                    OUTPUT_NAME "wx_${lib_toolkit}${lib_unicode}${lib_flavour}${lib_suffix}-${lib_version}"
                    OUTPUT_NAME_DEBUG "wx_${lib_toolkit}${lib_unicode}d${lib_flavour}${lib_suffix}-${lib_version}"
                    PREFIX "lib"
                )
        endif()

        if(wxBUILD_SHARED)
            # Add compiler type and or vendor
            set(dll_suffix "${lib_flavour}${lib_suffix}_${wxCOMPILER_PREFIX}")
            if(wxBUILD_VENDOR)
                wx_string_append(dll_suffix "_${wxBUILD_VENDOR}")
            endif()

            set(dll_version ${wxMAJOR_VERSION}${wxMINOR_VERSION})
            if(wxVERSION_IS_DEV)
                wx_string_append(dll_version ${wxRELEASE_NUMBER})
            endif()
            set_target_properties(${target_name}
                PROPERTIES
                    RUNTIME_OUTPUT_NAME "wx${lib_toolkit}${dll_version}${lib_unicode}${dll_suffix}"
                    RUNTIME_OUTPUT_NAME_DEBUG "wx${lib_toolkit}${dll_version}${lib_unicode}d${dll_suffix}"
                    PREFIX ""
                )
            target_compile_definitions(${target_name} PRIVATE
                "-DWXDLLNAME=wx${lib_toolkit}${dll_version}${lib_unicode}$<$<CONFIG:Debug>:d>${dll_suffix}")
        endif()
    else()
        set_target_properties(${target_name}
            PROPERTIES
                OUTPUT_NAME wx_${lib_toolkit}${lib_unicode}${lib_flavour}${lib_suffix}-${lib_version}
                # NOTE: wx-config can not be used to connect the libraries with the debug suffix.
                #OUTPUT_NAME_DEBUG wx_${lib_toolkit}${lib_unicode}d${lib_flavour}${lib_suffix}-${lib_version}
                OUTPUT_NAME_DEBUG wx_${lib_toolkit}${lib_unicode}${lib_flavour}${lib_suffix}-${lib_version}
            )
    endif()
    if(CYGWIN)
        target_link_libraries(${target_name} PUBLIC -L/usr/lib/w32api)
    endif()

    # Set common compile definitions
    target_compile_definitions(${target_name} PRIVATE WXBUILDING _LIB)
    if(target_name_short STREQUAL "mono" AND wxUSE_GUI)
        target_compile_definitions(${target_name} PRIVATE wxUSE_GUI=1 wxUSE_BASE=1)
    elseif(is_base OR NOT wxUSE_GUI)
        target_compile_definitions(${target_name} PRIVATE wxUSE_GUI=0 wxUSE_BASE=1)
    else()
        target_compile_definitions(${target_name} PRIVATE wxUSE_GUI=1 wxUSE_BASE=0)
    endif()

    if(wxUSE_UNICODE)
        if(WIN32)
            target_compile_definitions(${target_name} PUBLIC UNICODE)
        endif()
        target_compile_definitions(${target_name} PUBLIC _UNICODE)
    endif()

    if(WIN32 AND MSVC)
        # Suppress deprecation warnings for standard library calls
        target_compile_definitions(${target_name} PRIVATE
            _CRT_SECURE_NO_DEPRECATE=1
            _CRT_NON_CONFORMING_SWPRINTFS=1
            _SCL_SECURE_NO_WARNINGS=1
            _WINSOCK_DEPRECATED_NO_WARNINGS=1
            )
    endif()

    target_include_directories(${target_name}
        BEFORE
        PUBLIC
            ${wxSETUP_HEADER_PATH}
            ${wxSOURCE_DIR}/include
        )

    if(wxTOOLKIT_INCLUDE_DIRS)
        target_include_directories(${target_name}
            PUBLIC ${wxTOOLKIT_INCLUDE_DIRS})
    endif()

    if (WIN32)
        set(WIN32_LIBRARIES
            kernel32
            user32
            gdi32
            comdlg32
            winspool
            winmm
            shell32
            shlwapi
            comctl32
            ole32
            oleaut32
            uuid
            rpcrt4
            advapi32
            version
            wsock32
            wininet
            oleacc
            uxtheme
        )
        target_link_libraries(${target_name}
            PUBLIC ${WIN32_LIBRARIES})
    endif()

    if(wxTOOLKIT_LIBRARIES)
        target_link_libraries(${target_name}
            PUBLIC ${wxTOOLKIT_LIBRARIES})
    endif()
    target_compile_definitions(${target_name}
        PUBLIC ${wxTOOLKIT_DEFINITIONS})

    if(wxBUILD_SHARED)
        string(TOUPPER ${target_name_short} target_name_upper)
        if(target_name_short STREQUAL "mono")
            target_compile_definitions(${target_name} PRIVATE DLL_EXPORTS WXMAKINGDLL)
        else()
            target_compile_definitions(${target_name} PRIVATE DLL_EXPORTS WXMAKINGDLL_${target_name_upper})
        endif()
        if(NOT target_name_short STREQUAL "base")
            target_compile_definitions(${target_name} PRIVATE WXUSINGDLL)
        endif()
    endif()

    # Link common libraries
    if(NOT target_name_short STREQUAL "mono")
        if(NOT target_name_short STREQUAL "base")
            # All libraries except base need the base library
            target_link_libraries(${target_name} PUBLIC wxbase)
        endif()
        if(NOT is_base AND NOT target_name_short STREQUAL "core")
            # All non base libraries except core need core
            target_link_libraries(${target_name} PUBLIC wxcore)
        endif()
    endif()

    set_target_properties(${target_name} PROPERTIES FOLDER Libraries)

    wx_set_common_target_properties(${target_name})
endfunction()

# List of libraries added via wx_add_library() to use for wx-config
set(wxLIB_TARGETS)

# Add a wxWidgets library
# wx_add_library(<target_name> [IS_BASE] <src_files>...)
# first parameter is the name of the library
# if the second parameter is set to IS_BASE a non UI lib is created
# all additional parameters are source files for the library
macro(wx_add_library name)
    cmake_parse_arguments(wxADD_LIBRARY "IS_BASE" "" "" ${ARGN})
    set(src_files ${wxADD_LIBRARY_UNPARSED_ARGUMENTS})

    list(APPEND wxLIB_TARGETS ${name})
    set(wxLIB_TARGETS ${wxLIB_TARGETS} PARENT_SCOPE)

    if(wxBUILD_MONOLITHIC AND NOT ${name} STREQUAL "wxmono")
        # collect all source files for mono library
        set(wxMONO_SRC_FILES ${wxMONO_SRC_FILES} ${src_files} PARENT_SCOPE)
    else()
        list(APPEND src_files ${wxSETUP_HEADER_FILE})

        if(wxBUILD_SHARED)
            set(wxBUILD_LIB_TYPE SHARED)
            if(WIN32)
                # Add WIN32 version information
                list(APPEND src_files "${wxSOURCE_DIR}/src/msw/version.rc" "${wxSOURCE_DIR}/include/wx/msw/genrcdefs.h")
            endif()
        else()
            set(wxBUILD_LIB_TYPE STATIC)
        endif()

        if(${name} MATCHES "wx.*")
            string(SUBSTRING ${name} 2 -1 name_short)
        else()
            set(name_short ${name})
        endif()

        add_library(${name} ${wxBUILD_LIB_TYPE} ${src_files})
        add_library(wx::${name_short} ALIAS ${name})
        wx_set_target_properties(${name} ${wxADD_LIBRARY_IS_BASE})
        set_property(TARGET ${name} PROPERTY PROJECT_LABEL ${name_short})

        # Setup install
        wx_install(TARGETS ${name}
            LIBRARY DESTINATION "lib${wxPLATFORM_LIB_DIR}"
            ARCHIVE DESTINATION "lib${wxPLATFORM_LIB_DIR}"
            RUNTIME DESTINATION "lib${wxPLATFORM_LIB_DIR}"
            BUNDLE DESTINATION Applications/wxWidgets
            )
    endif()
endmacro()

# Enable cotire for target, use optional second argument for prec. header
macro(wx_target_enable_precomp target_name)
    target_compile_definitions(${target_name} PRIVATE WX_PRECOMP)
    if(NOT ${ARGV1} STREQUAL "")
        set_target_properties(${target_name} PROPERTIES
            COTIRE_CXX_PREFIX_HEADER_INIT ${ARGV1})
    endif()
    set_target_properties(${target_name} PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
    cotire(${target_name})
endmacro()

# Enable precompiled headers for tests
macro(wx_test_enable_precomp target_name)
    if(wxBUILD_PRECOMP)
        wx_target_enable_precomp(${target_name} "${wxSOURCE_DIR}/tests/testprec.h")
    elseif(MSVC)
        target_compile_definitions(${target_name} PRIVATE NOPCH)
    endif()
endmacro()

# Enable precompiled headers for samples
macro(wx_sample_enable_precomp target_name)
    if(wxBUILD_PRECOMP)
        wx_target_enable_precomp(${target_name} "${wxSOURCE_DIR}/include/wx/wxprec.h")
    elseif(MSVC)
        target_compile_definitions(${target_name} PRIVATE NOPCH)
    endif()
endmacro()

# Enable precompiled headers for wx libraries
macro(wx_finalize_lib target_name)
    if(wxBUILD_PRECOMP)
        if(TARGET ${target_name})
            wx_target_enable_precomp(${target_name} "${wxSOURCE_DIR}/include/wx/wxprec.h")
        endif()
    elseif(MSVC)
        wx_lib_compile_definitions(${target_name} PRIVATE NOPCH)
    endif()
endmacro()

# wx_lib_link_libraries(name [])
# Forwards everything to target_link_libraries() except for monolithic
# build where it collects all libraries for linking with the mono lib
macro(wx_lib_link_libraries name)
    if(wxBUILD_MONOLITHIC)
        cmake_parse_arguments(_LIB_LINK "" "" "PUBLIC;PRIVATE" ${ARGN})
        list(APPEND wxMONO_LIBS_PUBLIC ${_LIB_LINK_PUBLIC})
        list(APPEND wxMONO_LIBS_PRIVATE ${_LIB_LINK_PRIVATE})
        set(wxMONO_LIBS_PUBLIC ${wxMONO_LIBS_PUBLIC} PARENT_SCOPE)
        set(wxMONO_LIBS_PRIVATE ${wxMONO_LIBS_PRIVATE} PARENT_SCOPE)
    else()
        target_link_libraries(${name};${ARGN})
    endif()
endmacro()

# wx_exe_link_libraries(target libs...)
# Link wx libraries to executable
macro(wx_exe_link_libraries name)
    if(wxBUILD_MONOLITHIC)
        target_link_libraries(${name} PUBLIC wxmono)
    else()
        target_link_libraries(${name};PRIVATE;${ARGN})
    endif()
endmacro()

# wx_lib_include_directories(name [])
# Forwards everything to target_include_directories() except for monolithic
# build where it collects all include paths for linking with the mono lib
macro(wx_lib_include_directories name)
    cmake_parse_arguments(_LIB_INCLUDE_DIRS "" "" "PUBLIC;PRIVATE" ${ARGN})
    if(wxBUILD_MONOLITHIC)
        list(APPEND wxMONO_INCLUDE_DIRS_PUBLIC ${_LIB_INCLUDE_DIRS_PUBLIC})
        list(APPEND wxMONO_INCLUDE_DIRS_PRIVATE ${_LIB_INCLUDE_DIRS_PRIVATE})
        set(wxMONO_INCLUDE_DIRS_PUBLIC ${wxMONO_INCLUDE_DIRS_PUBLIC} PARENT_SCOPE)
        set(wxMONO_INCLUDE_DIRS_PRIVATE ${wxMONO_INCLUDE_DIRS_PRIVATE} PARENT_SCOPE)
    else()
        set(INCLUDE_POS)
        if (_LIB_INCLUDE_DIRS_PRIVATE)
            set(INCLUDE_POS BEFORE)
        endif()
        target_include_directories(${name};${INCLUDE_POS};${ARGN})
    endif()
endmacro()

# wx_lib_compile_definitions(name [])
# Forwards everything to target_compile_definitions() except for monolithic
# build where it collects all definitions for linking with the mono lib
macro(wx_lib_compile_definitions name)
    if(wxBUILD_MONOLITHIC)
        cmake_parse_arguments(_LIB_DEFINITIONS "" "" "PUBLIC;PRIVATE" ${ARGN})
        list(APPEND wxMONO_DEFINITIONS_PUBLIC ${_LIB_DEFINITIONS_PUBLIC})
        list(APPEND wxMONO_DEFINITIONS_PRIVATE ${_LIB_DEFINITIONS_PRIVATE})
        set(wxMONO_DEFINITIONS_PUBLIC ${wxMONO_DEFINITIONS_PUBLIC} PARENT_SCOPE)
        set(wxMONO_DEFINITIONS_PRIVATE ${wxMONO_DEFINITIONS_PRIVATE} PARENT_SCOPE)
    else()
        target_compile_definitions(${name};${ARGN})
    endif()
endmacro()

# Set common properties for a builtin third party library
function(wx_set_builtin_target_properties target_name)
    if(wxUSE_UNICODE AND target_name STREQUAL "wxregex")
        set(lib_unicode u)
    else()
        set(lib_unicode)
    endif()
    if(NOT WIN32)
        set(postfix -${wxMAJOR_VERSION}.${wxMINOR_VERSION})
    endif()
    set_target_properties(${target_name}
        PROPERTIES
            OUTPUT_NAME ${target_name}${lib_unicode}${postfix}
        )
    if(WIN32)
        set_target_properties(${target_name}
            PROPERTIES
                OUTPUT_NAME_DEBUG ${target_name}${lib_unicode}d
        )
    endif()

    if(wxUSE_UNICODE)
        if(WIN32)
            target_compile_definitions(${target_name} PUBLIC UNICODE)
        endif()
        target_compile_definitions(${target_name} PUBLIC _UNICODE)
    endif()

    if(MSVC)
        # we're not interested in deprecation warnings about the use of
        # standard C functions in the 3rd party libraries (these warnings
        # are only given by VC8+ but it's simpler to just always define
        # this symbol which disables them, even for previous VC versions)
        target_compile_definitions(${target_name} PRIVATE
            _CRT_SECURE_NO_DEPRECATE=1
            _SCL_SECURE_NO_WARNINGS=1
        )
    endif()

    target_include_directories(${target_name}
        BEFORE
        PUBLIC
            ${wxSETUP_HEADER_PATH}
        )

    set_target_properties(${target_name} PROPERTIES FOLDER "Third Party Libraries")

    wx_set_common_target_properties(${target_name} DEFAULT_WARNINGS)
    if(NOT wxBUILD_SHARED)
        wx_install(TARGETS ${name} ARCHIVE DESTINATION "lib${wxPLATFORM_LIB_DIR}")
    endif()
endfunction()

# Add a third party builtin library
function(wx_add_builtin_library name)
    wx_list_add_prefix(src_list "${wxSOURCE_DIR}/" ${ARGN})

    if(${name} MATCHES "wx.*")
        string(SUBSTRING ${name} 2 -1 name_short)
    else()
        set(name_short ${name})
    endif()

    add_library(${name} STATIC ${src_list})
    add_library(wx::${name_short} ALIAS ${name})
    wx_set_builtin_target_properties(${name})
    set_property(TARGET ${name} PROPERTY PROJECT_LABEL ${name_short})
    if(wxBUILD_SHARED)
        set_target_properties(${name} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
    endif()
endfunction()

# List of third party libraries added via wx_add_thirdparty_library()
# to display in configuration summary
set(wxTHIRD_PARTY_LIBRARIES)

# Add third party library
function(wx_add_thirdparty_library var_name lib_name help_str)
    cmake_parse_arguments(THIRDPARTY "" "DEFAULT;DEFAULT_APPLE;DEFAULT_WIN32" "" ${ARGN})

    if(THIRDPARTY_DEFAULT)
        set(thirdparty_lib_default ${THIRDPARTY_DEFAULT})
    elseif(THIRDPARTY_DEFAULT_APPLE AND APPLE)
        set(thirdparty_lib_default ${THIRDPARTY_DEFAULT_APPLE})
    elseif(THIRDPARTY_DEFAULT_WIN32 AND WIN32)
        set(thirdparty_lib_default ${THIRDPARTY_DEFAULT_WIN32})
    elseif(UNIX AND NOT APPLE)
        # Try sys libraries for MSYS and CYGWIN
        set(thirdparty_lib_default sys)
    elseif(WIN32 OR APPLE)
        # On Windows or apple platforms prefer using the builtin libraries
        set(thirdparty_lib_default builtin)
    else()
        set(thirdparty_lib_default sys)
    endif()

    wx_option(${var_name} ${help_str} ${thirdparty_lib_default}
        STRINGS builtin sys OFF)

    if(${var_name} STREQUAL "sys")
        # If the sys library can not be found use builtin
        find_package(${lib_name})
        string(TOUPPER ${lib_name} lib_name_upper)
        if(NOT ${${lib_name_upper}_FOUND})
            wx_option_force_value(${var_name} builtin)
        endif()
    endif()

    if(${var_name} STREQUAL "builtin" AND NOT wxBUILD_SHARED)
        # Only install if we build as static libraries
        wx_install(TARGETS ${target_name}
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            )
    endif()

    set(wxTHIRD_PARTY_LIBRARIES ${wxTHIRD_PARTY_LIBRARIES} ${var_name} "${help_str}" PARENT_SCOPE)
endfunction()

function(wx_print_thirdparty_library_summary)
    set(nameLength 0)
    set(nameValLength 0)
    set(var_name)
    foreach(entry IN LISTS wxTHIRD_PARTY_LIBRARIES)
        if(NOT var_name)
            set(var_name ${entry})
        else()
            string(LENGTH ${var_name} len)
            if(len GREATER nameLength)
                set(nameLength ${len})
            endif()
            string(LENGTH ${${var_name}} len)
            if(len GREATER nameValLength)
                set(nameValLength ${len})
            endif()
            set(var_name)
        endif()
    endforeach()
    math(EXPR nameLength "${nameLength}+1") # account for :

    set(message "Which libraries should wxWidgets use?\n")
    foreach(entry IN LISTS wxTHIRD_PARTY_LIBRARIES)
        if(NOT var_name)
            set(var_name ${entry})
        else()
            set(namestr "${var_name}:          ")
            set(nameval "${${var_name}}          ")
            string(SUBSTRING ${namestr} 0 ${nameLength} namestr)
            string(SUBSTRING ${nameval} 0 ${nameValLength} nameval)
            wx_string_append(message "    ${namestr}  ${nameval}  (${entry})\n")
            set(var_name)
        endif()
    endforeach()
    message(STATUS ${message})
endfunction()

# Add executable for sample
# wx_add_sample(<name> [CONSOLE|DLL] [IMPORTANT] [SRC_FILES...]
#    [LIBRARIES ...] [NAME target_name] [FOLDER folder])
# first parameter may be CONSOLE to indicate a console application
# all following parameters a src files for the executable
# source files are relative to samples/${name}/
# Optionally:
# DATA followed by required data files. Use a colon to separate different source and dest paths
# DEFINITIONS list of definitions for the target
# FOLDER subfolder in IDE
# LIBRARIES followed by required libraries
# NAME alternative target_name
# IMPORTANT does not require wxBUILD_SAMPLES=ALL
# RES followed by WIN32 .rc files
#
# Additionally the following variables may be set before calling wx_add_sample:
# wxSAMPLE_SUBDIR subdirectory in the samples/ folder to use as base
# wxSAMPLE_FOLDER IDE sub folder to be used for the samples
function(wx_add_sample name)
    cmake_parse_arguments(SAMPLE
        "CONSOLE;DLL;IMPORTANT"
        "NAME;FOLDER"
        "DATA;DEFINITIONS;DEPENDS;LIBRARIES;RES"
        ${ARGN}
        )
    if(NOT SAMPLE_FOLDER AND wxSAMPLE_FOLDER)
        set(SAMPLE_FOLDER ${wxSAMPLE_FOLDER})
    endif()

    # Only build important samples without wxBUILD_SAMPLES=ALL
    if(NOT SAMPLE_IMPORTANT AND NOT wxBUILD_SAMPLES STREQUAL "ALL")
        return()
    endif()
    foreach(depend ${SAMPLE_DEPENDS})
        if(NOT ${depend})
            return()
        endif()
    endforeach()

    # Only build GUI samples with wxUSE_GUI=1
    if(NOT wxUSE_GUI AND NOT SAMPLE_CONSOLE)
        return()
    endif()

    if(SAMPLE_UNPARSED_ARGUMENTS)
        wx_list_add_prefix(src_files
            "${wxSOURCE_DIR}/samples/${wxSAMPLE_SUBDIR}${name}/"
            ${SAMPLE_UNPARSED_ARGUMENTS})
    else()
        # If no source files have been specified use default src name
        set(src_files ${wxSOURCE_DIR}/samples/${wxSAMPLE_SUBDIR}${name}/${name}.cpp)
    endif()

    if(WIN32)
        if(SAMPLE_RES)
            foreach(res ${SAMPLE_RES})
                list(APPEND src_files ${wxSOURCE_DIR}/samples/${wxSAMPLE_SUBDIR}${name}/${res})
            endforeach()
        else()
            # Include default sample.rc
            list(APPEND src_files ${wxSOURCE_DIR}/samples/sample.rc)
        endif()
    elseif(APPLE AND NOT IPHONE)
        list(APPEND src_files ${wxSOURCE_DIR}/src/osx/carbon/wxmac.icns)
    endif()

    if(SAMPLE_NAME)
        set(target_name ${SAMPLE_NAME})
    else()
        set(target_name ${name})
    endif()

    if(SAMPLE_DLL)
        add_library(${target_name} SHARED ${src_files})
    else()
        if(SAMPLE_CONSOLE)
            set(exe_type)
        else()
            set(exe_type WIN32 MACOSX_BUNDLE)
        endif()

        if (WXMSW AND DEFINED wxUSE_DPI_AWARE_MANIFEST)
            set(wxDPI_MANIFEST_PRFIX "wx")
            if (wxARCH_SUFFIX)
                set(wxDPI_MANIFEST_PRFIX "amd64")
            endif()
            set(wxUSE_DPI_AWARE_MANIFEST_VALUE 0)
            if (${wxUSE_DPI_AWARE_MANIFEST} MATCHES "system")
                set(wxUSE_DPI_AWARE_MANIFEST_VALUE 1)
                list(APPEND src_files "${wxSOURCE_DIR}/include/wx/msw/${wxDPI_MANIFEST_PRFIX}_dpi_aware.manifest")
            elseif(${wxUSE_DPI_AWARE_MANIFEST} MATCHES "per-monitor")
                set(wxUSE_DPI_AWARE_MANIFEST_VALUE 2)
                list(APPEND src_files "${wxSOURCE_DIR}/include/wx/msw/${wxDPI_MANIFEST_PRFIX}_dpi_aware_pmv2.manifest")
            endif()
        endif()

        add_executable(${target_name} ${exe_type} ${src_files})

        if (DEFINED wxUSE_DPI_AWARE_MANIFEST_VALUE)
            target_compile_definitions(${target_name} PRIVATE wxUSE_DPI_AWARE_MANIFEST=${wxUSE_DPI_AWARE_MANIFEST_VALUE})
        endif()
    endif()
    # All samples use at least the base library other libraries
    # will have to be added with wx_link_sample_libraries()
    wx_exe_link_libraries(${target_name} wxbase)
    if(NOT SAMPLE_CONSOLE)
        # UI samples always require core
        wx_exe_link_libraries(${target_name} wxcore)
    else()
        target_compile_definitions(${target_name} PRIVATE wxUSE_GUI=0 wxUSE_BASE=1)
    endif()
    if(SAMPLE_LIBRARIES)
        wx_exe_link_libraries(${target_name} ${SAMPLE_LIBRARIES})
    endif()
    if(wxBUILD_SHARED)
        target_compile_definitions(${target_name} PRIVATE WXUSINGDLL)
    endif()
    if(SAMPLE_DEFINITIONS)
        target_compile_definitions(${target_name} PRIVATE ${SAMPLE_DEFINITIONS})
    endif()
    if(SAMPLE_DATA)
        # TODO: handle data files differently for OS X bundles
        # Copy data files to output directory
        foreach(data_src ${SAMPLE_DATA})
            string(FIND ${data_src} ":" HAS_COLON)
            if(${HAS_COLON} GREATER -1)
                MATH(EXPR DEST_INDEX "${HAS_COLON}+1")
                string(SUBSTRING ${data_src} ${DEST_INDEX} -1 data_dst)
                string(SUBSTRING ${data_src} 0 ${HAS_COLON} data_src)
            else()
                set(data_dst ${data_src})
            endif()

            list(APPEND cmds COMMAND ${CMAKE_COMMAND}
                -E copy ${wxSOURCE_DIR}/samples/${wxSAMPLE_SUBDIR}${name}/${data_src}
                ${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}/${data_dst})
        endforeach()
        add_custom_command(
            TARGET ${target_name} ${cmds}
            COMMENT "Copying sample data files...")
    endif()
    if(WIN32)
        # The resource compiler needs this include directory to find res files
        target_include_directories(${target_name} PRIVATE ${wxSOURCE_DIR}/samples/)
    elseif(APPLE)
        if(NOT IPHONE)
            set_target_properties(${target_name} PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST "${wxSOURCE_DIR}/samples/Info.plist.in"
                RESOURCE "${wxSOURCE_DIR}/src/osx/carbon/wxmac.icns")
        endif()
        set_target_properties(${target_name} PROPERTIES
            MACOSX_BUNDLE_ICON_FILE wxmac.icns
            MACOSX_BUNDLE_LONG_VERSION_STRING "${wxVERSION}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${wxVERSION}"
            MACOSX_BUNDLE_VERSION "${wxVERSION}"
            MACOSX_BUNDLE_COPYRIGHT "${wxCOPYRIGHT}"
            MACOSX_BUNDLE_GUI_IDENTIFIER "org.wxwidgets.${target_name}"
            )
    endif()

    set(folder "Samples")
    if(SAMPLE_FOLDER)
        wx_string_append(folder "/${SAMPLE_FOLDER}")
    endif()
    wx_set_common_target_properties(${target_name})
    wx_sample_enable_precomp(${target_name})
    set_target_properties(${target_name} PROPERTIES
        FOLDER ${folder}
        )
    set_target_properties(${target_name} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${wxOUTPUT_DIR}/${wxCOMPILER_PREFIX}${wxARCH_SUFFIX}_${lib_suffix}"
        )
endfunction()

# Link libraries to a sample
function(wx_link_sample_libraries name)
    if(TARGET ${name})
        target_link_libraries(${name} PUBLIC ${ARGN})
    endif()
endfunction()

# Add a option and mark is as advanced if it starts with wxUSE_
# wx_option(<name> <desc> [default] [STRINGS strings])
# The default is ON if not third parameter is specified
function(wx_option name desc)
    cmake_parse_arguments(OPTION "" "" "STRINGS" ${ARGN})
    if(ARGC EQUAL 2)
        set(default ON)
    else()
        set(default ${OPTION_UNPARSED_ARGUMENTS})
    endif()

    if(OPTION_STRINGS)
        set(cache_type STRING)
    else()
        set(cache_type BOOL)
    endif()

    set(${name} "${default}" CACHE ${cache_type} "${desc}")
    string(SUBSTRING ${name} 0 6 prefix)
    if(prefix STREQUAL "wxUSE_")
        mark_as_advanced(${name})
    endif()
    if(OPTION_STRINGS)
        set_property(CACHE ${name} PROPERTY STRINGS ${OPTION_STRINGS})
        # Check valid value
        set(value_is_valid FALSE)
        set(avail_values)
        foreach(opt ${OPTION_STRINGS})
            if(${name} STREQUAL opt)
                set(value_is_valid TRUE)
                break()
            endif()
            wx_string_append(avail_values " ${opt}")
        endforeach()
        if(NOT value_is_valid)
            message(FATAL_ERROR "Invalid value \"${${name}}\" for option ${name}. Valid values are: ${avail_values}")
        endif()
    endif()
endfunction()

# Force a new value for an option created with wx_option
function(wx_option_force_value name value)
    get_property(helpstring CACHE ${name} PROPERTY HELPSTRING)
    get_property(type CACHE ${name} PROPERTY TYPE)
    set(${name} ${value} CACHE ${type} ${helpstring} FORCE)
endfunction()

macro(wx_dependent_option option doc default depends force)
  if(${option}_ISSET MATCHES "^${option}_ISSET$")
    set(${option}_AVAILABLE 1)
    foreach(d ${depends})
      string(REGEX REPLACE " +" ";" CMAKE_DEPENDENT_OPTION_DEP "${d}")
      if(${CMAKE_DEPENDENT_OPTION_DEP})
      else()
        set(${option}_AVAILABLE 0)
      endif()
    endforeach()
    if(${option}_AVAILABLE)
      wx_option(${option} "${doc}" "${default}")
      set(${option} "${${option}}" CACHE BOOL "${doc}" FORCE)
    else()
      if(${option} MATCHES "^${option}$")
      else()
        set(${option} "${${option}}" CACHE INTERNAL "${doc}")
      endif()
      set(${option} ${force})
    endif()
  else()
    set(${option} "${${option}_ISSET}")
  endif()
endmacro()

# wx_add_test(<name> [src...])
# Optionally:
# DATA followed by required data files
# RES followed by WIN32 .rc files
function(wx_add_test name)
    cmake_parse_arguments(TEST "" "" "DATA;RES" ${ARGN})
    wx_list_add_prefix(test_src "${wxSOURCE_DIR}/tests/" ${TEST_UNPARSED_ARGUMENTS})
    if(WIN32 AND TEST_RES)
        foreach(res ${TEST_RES})
            list(APPEND test_src ${wxSOURCE_DIR}/tests/${res})
        endforeach()
    endif()
    add_executable(${name} ${test_src})
    target_include_directories(${name} PRIVATE "${wxSOURCE_DIR}/tests" "${wxSOURCE_DIR}/3rdparty/catch/include")
    wx_exe_link_libraries(${name} wxbase)
    if(wxBUILD_SHARED)
        target_compile_definitions(${name} PRIVATE WXUSINGDLL)
    endif()
    if(TEST_DATA)
        # Copy data files to output directory
        foreach(data_file ${TEST_DATA})
            list(APPEND cmds COMMAND ${CMAKE_COMMAND}
                -E copy ${wxSOURCE_DIR}/tests/${data_file}
                ${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}/${data_file})
        endforeach()
        add_custom_command(
            TARGET ${name} ${cmds}
            COMMENT "Copying test data files...")
    endif()
    wx_set_common_target_properties(${name})
    set_target_properties(${name} PROPERTIES FOLDER "Tests")
    set_target_properties(${name} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${wxSOURCE_DIR}/tests"
        )

    add_test(NAME ${name}
        COMMAND ${name}
        WORKING_DIRECTORY ${wxSOURCE_DIR}/tests)
endfunction()

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
include(CMakePrintHelpers)

# Use the MSVC/makefile naming convention, or the configure naming convention,
# this is the same check as used in FindwxWidgets.
if(WIN32 AND NOT CYGWIN AND NOT MSYS)
    set(WIN32_MSVC_NAMING 1)
else()
    set(WIN32_MSVC_NAMING 0)
endif()


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

# Get a valid flavour name with optional prefix
macro(wx_get_flavour flavour prefix)
    if(wxBUILD_FLAVOUR)
        set(flav ${wxBUILD_FLAVOUR})
        string(REPLACE "-" "_" flav ${flav})
        set(${flavour} "${prefix}${flav}")
    else()
        set(${flavour})
    endif()
endmacro()

if(WIN32_MSVC_NAMING)
    # Generator expression to not create different Debug and Release directories
    set(GEN_EXPR_DIR "$<1:/>")
    set(wxINSTALL_INCLUDE_DIR "include")
else()
    set(GEN_EXPR_DIR "/")
    wx_get_flavour(lib_flavour "-")
    set(wxINSTALL_INCLUDE_DIR "include/wx-${wxMAJOR_VERSION}.${wxMINOR_VERSION}${lib_flavour}")
endif()

# Set properties common to builtin third party libraries and wx libs
function(wx_set_common_target_properties target_name)
    cmake_parse_arguments(wxCOMMON_TARGET_PROPS "DEFAULT_WARNINGS" "" "" ${ARGN})

    set_target_properties(${target_name} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${wxOUTPUT_DIR}${GEN_EXPR_DIR}${wxPLATFORM_LIB_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${wxOUTPUT_DIR}${GEN_EXPR_DIR}${wxPLATFORM_LIB_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${wxOUTPUT_DIR}${GEN_EXPR_DIR}${wxPLATFORM_LIB_DIR}"
        )

    if(wxBUILD_PIC)
        set_target_properties(${target_name} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
    endif()

    if(MSVC)
        if(wxCOMMON_TARGET_PROPS_DEFAULT_WARNINGS)
            set(MSVC_WARNING_LEVEL "/W3")
        else()
            set(MSVC_WARNING_LEVEL "/W4")
        endif()
        target_compile_options(${target_name} PRIVATE ${MSVC_WARNING_LEVEL})
    elseif(NOT wxCOMMON_TARGET_PROPS_DEFAULT_WARNINGS)
        set(common_gcc_clang_compile_options
            -Wall
            -Wundef
            -Wunused-parameter
        )
        set(common_gcc_clang_cpp_compile_options
            -Wno-ctor-dtor-privacy
            -Woverloaded-virtual
        )

        if(WXOSX_COCOA)
            # when building using Cocoa we currently get tons of deprecation
            # warnings from the standard headers -- disable them as we already know
            # that they're deprecated but we still have to use them to support older
            # toolkit versions and leaving this warning enabled prevents seeing any
            # other ones
            list(APPEND common_gcc_clang_compile_options
                -Wno-deprecated-declarations
            )
        endif()

        if(APPLE)
            # Xcode automatically adds -Wshorten-64-to-32 to C++ flags
            # and it causes a lot of warnings in wx code
            list(APPEND common_gcc_clang_compile_options
                -Wno-shorten-64-to-32
            )
        endif()

        if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
            list(APPEND common_gcc_clang_compile_options
                -Wno-ignored-attributes
            )
        endif()

        target_compile_options(${target_name} PRIVATE
            ${common_gcc_clang_compile_options}
            $<$<COMPILE_LANGUAGE:CXX>:${common_gcc_clang_cpp_compile_options}>
        )
    endif()

    if(wxUSE_NO_RTTI)
        if(MSVC)
            target_compile_options(${target_name} PRIVATE "/GR-")
        elseif(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
            target_compile_options(${target_name} PRIVATE "-fno-rtti")
        endif()
        target_compile_definitions(${target_name} PRIVATE "-DwxNO_RTTI")
    endif()

    if(wxBUILD_LARGEFILE_SUPPORT)
        target_compile_definitions(${target_name} PUBLIC "-D_FILE_OFFSET_BITS=64")
    endif()

    if(CMAKE_USE_PTHREADS_INIT)
        target_compile_options(${target_name} PRIVATE "-pthread")
        # clang++.exe: warning: argument unused during compilation: '-pthread' [-Wunused-command-line-argument]
        if(NOT (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
            set_target_properties(${target_name} PROPERTIES LINK_FLAGS "-pthread")
        endif()
    endif()
    wx_set_source_groups()
endfunction()

# Set common properties on wx library target
function(wx_set_target_properties target_name)
    cmake_parse_arguments(wxTARGET "IS_BASE;IS_PLUGIN;IS_MONO" "" "" ${ARGN})
    if(${target_name} MATCHES "wx.*")
        string(SUBSTRING ${target_name} 2 -1 target_name_short)
    else()
        set(target_name_short ${target_name})
    endif()

    # Set library name according to:
    # docs/contributing/about-platform-toolkit-and-library-names.md
    if(wxTARGET_IS_BASE)
        set(lib_toolkit base)
    else()
        set(lib_toolkit ${wxBUILD_TOOLKIT}${wxBUILD_WIDGETSET})
    endif()

    if(WIN32_MSVC_NAMING)
        set(lib_version ${wxMAJOR_VERSION}${wxMINOR_VERSION})
    else()
        set(lib_version ${wxMAJOR_VERSION}.${wxMINOR_VERSION})
    endif()
    set(dll_version ${wxMAJOR_VERSION}${wxMINOR_VERSION})
    if(wxVERSION_IS_DEV)
        wx_string_append(dll_version ${wxRELEASE_NUMBER})
    endif()

    set(lib_unicode)
    set(lib_unicode "u")

    set(lib_rls)
    set(lib_dbg)
    set(lib_gen)
    if(WIN32_MSVC_NAMING)
        set(lib_dbg "d")
        set(lib_gen "$<$<CONFIG:Debug>:${lib_dbg}>")
    endif()

    set(lib_suffix)
    if(NOT target_name_short STREQUAL "base" AND NOT wxTARGET_IS_MONO)
        # Do not append library name for base or mono library
        set(lib_suffix "_${target_name_short}")
    endif()
    wx_get_flavour(lib_flavour "_")
    set(lib_suffix "${lib_flavour}${lib_suffix}")

    set(dll_suffix "${lib_suffix}")
    if(wxCOMPILER_PREFIX)
        wx_string_append(dll_suffix "_${wxCOMPILER_PREFIX}")
    endif()
    # For compatibility with MSVS project files and makefile.vc, use arch
    # suffix for non-x86 (including x86_64) DLLs.
    if(MSVC AND wxARCH_SUFFIX)
        # This one already includes the leading underscore, so don't add another one.
        wx_string_append(dll_suffix "${wxARCH_SUFFIX}")
    endif()
    if(wxBUILD_VENDOR)
        wx_string_append(dll_suffix "_${wxBUILD_VENDOR}")
    endif()

    set(cross_target)
    if (CMAKE_CROSSCOMPILING)
        set(cross_target "-${CMAKE_SYSTEM_NAME}")
    endif()

    set(lib_prefix "lib")
    if(MSVC OR (WIN32 AND wxBUILD_SHARED))
        set(lib_prefix)
    endif()

    # static (and import) library names
    if(WIN32_MSVC_NAMING)
        # match msvc/makefile output name
        set(wxOUTPUT_NAME       "wx${lib_toolkit}${lib_version}${lib_unicode}${lib_rls}${lib_suffix}")
        set(wxOUTPUT_NAME_DEBUG "wx${lib_toolkit}${lib_version}${lib_unicode}${lib_dbg}${lib_suffix}")
    else()
        # match configure output name
        set(wxOUTPUT_NAME       "wx_${lib_toolkit}${lib_unicode}${lib_rls}${lib_suffix}-${lib_version}${cross_target}")
        set(wxOUTPUT_NAME_DEBUG "wx_${lib_toolkit}${lib_unicode}${lib_dbg}${lib_suffix}-${lib_version}${cross_target}")
    endif()

    # shared library names
    if(WIN32)
        # msvc/makefile/configure use the same format on Windows
        set(wxRUNTIME_OUTPUT_NAME       "wx${lib_toolkit}${dll_version}${lib_unicode}${lib_rls}${dll_suffix}")
        set(wxRUNTIME_OUTPUT_NAME_DEBUG "wx${lib_toolkit}${dll_version}${lib_unicode}${lib_dbg}${dll_suffix}")
        set(wxDLLNAME                   "wx${lib_toolkit}${dll_version}${lib_unicode}${lib_gen}${dll_suffix}")
    else()
        # match configure on linux/mac
        set(wxRUNTIME_OUTPUT_NAME       "wx_${lib_toolkit}${lib_unicode}${lib_rls}${dll_suffix}-${lib_version}${cross_target}")
        set(wxRUNTIME_OUTPUT_NAME_DEBUG "wx_${lib_toolkit}${lib_unicode}${lib_dbg}${dll_suffix}-${lib_version}${cross_target}")
        set(wxDLLNAME                   "wx_${lib_toolkit}${lib_unicode}${lib_gen}${dll_suffix}-${lib_version}${cross_target}")
    endif()

    set_target_properties(${target_name} PROPERTIES
        OUTPUT_NAME               "${wxOUTPUT_NAME}"
        OUTPUT_NAME_DEBUG         "${wxOUTPUT_NAME_DEBUG}"
        RUNTIME_OUTPUT_NAME       "${wxRUNTIME_OUTPUT_NAME}"
        RUNTIME_OUTPUT_NAME_DEBUG "${wxRUNTIME_OUTPUT_NAME_DEBUG}"
        PREFIX                    "${lib_prefix}"
    )

    if(WIN32_MSVC_NAMING AND NOT MSVC)
        # match makefile.gcc, use .a instead of .dll.a for import libraries
        set_target_properties(${target_name} PROPERTIES IMPORT_SUFFIX ".a")
    endif()

    if(wxBUILD_SHARED)
        target_compile_definitions(${target_name} PRIVATE "WXDLLNAME=${wxDLLNAME}")
    endif()

    if(CYGWIN)
        target_link_libraries(${target_name} PUBLIC -L/usr/lib/w32api)
    endif()

    # Set common compile definitions
    target_compile_definitions(${target_name} PRIVATE WXBUILDING)
    if(wxTARGET_IS_MONO AND wxUSE_GUI)
        target_compile_definitions(${target_name} PRIVATE wxUSE_GUI=1 wxUSE_BASE=1)
    elseif(wxTARGET_IS_PLUGIN)
        target_compile_definitions(${target_name} PRIVATE wxUSE_GUI=0 wxUSE_BASE=0)
    elseif(wxTARGET_IS_BASE OR NOT wxUSE_GUI)
        target_compile_definitions(${target_name} PRIVATE wxUSE_GUI=0 wxUSE_BASE=1)
    else()
        target_compile_definitions(${target_name} PRIVATE wxUSE_GUI=1 wxUSE_BASE=0)
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

    file(RELATIVE_PATH wxSETUP_HEADER_REL ${wxOUTPUT_DIR} ${wxSETUP_HEADER_PATH})
    target_include_directories(${target_name}
        BEFORE
        PUBLIC
            $<BUILD_INTERFACE:${wxSETUP_HEADER_PATH}>
            $<BUILD_INTERFACE:${wxSOURCE_DIR}/include>
            $<INSTALL_INTERFACE:lib/${wxSETUP_HEADER_REL}>
            $<INSTALL_INTERFACE:${wxINSTALL_INCLUDE_DIR}>
        )

    if(wxTOOLKIT_INCLUDE_DIRS AND NOT wxTARGET_IS_BASE)
        target_include_directories(${target_name}
            PRIVATE ${wxTOOLKIT_INCLUDE_DIRS})
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
            ws2_32
            wininet
            oleacc
            uxtheme
        )
        target_link_libraries(${target_name}
            PUBLIC ${WIN32_LIBRARIES})
    endif()

    if(wxTOOLKIT_LIBRARIES AND NOT wxTARGET_IS_BASE)
        target_link_libraries(${target_name}
            PUBLIC ${wxTOOLKIT_LIBRARIES})
    endif()
    target_compile_definitions(${target_name}
        PUBLIC ${wxTOOLKIT_DEFINITIONS})

    if(wxBUILD_SHARED)
        string(TOUPPER ${target_name_short} target_name_upper)
        if(wxTARGET_IS_MONO)
            target_compile_definitions(${target_name} PRIVATE WXMAKINGDLL)
        elseif(NOT wxTARGET_IS_PLUGIN)
            target_compile_definitions(${target_name} PRIVATE WXMAKINGDLL_${target_name_upper})
        endif()
        target_compile_definitions(${target_name} INTERFACE WXUSINGDLL)
    endif()
    if(wxTARGET_IS_PLUGIN OR (wxBUILD_SHARED AND NOT target_name_short STREQUAL "base"))
        target_compile_definitions(${target_name} PRIVATE WXUSINGDLL)
    endif()

    # Link common libraries
    if(NOT wxTARGET_IS_MONO AND NOT wxTARGET_IS_PLUGIN)
        if(NOT target_name_short STREQUAL "base")
            # All libraries except base need the base library
            target_link_libraries(${target_name} PUBLIC wxbase)
        endif()
        if(NOT wxTARGET_IS_BASE AND NOT target_name_short STREQUAL "core")
            # All non base libraries except core need core
            target_link_libraries(${target_name} PUBLIC wxcore)
        endif()
    endif()

    set_target_properties(${target_name} PROPERTIES FOLDER Libraries)

    set_target_properties(${target_name} PROPERTIES
        SOVERSION ${wxSOVERSION_MAJOR}
        VERSION ${wxSOVERSION}
    )

    wx_set_common_target_properties(${target_name})
endfunction()

# List of libraries added via wx_add_library() to use for wx-config
set(wxLIB_TARGETS)

# Add a wxWidgets library
# wx_add_library(<target_name> [IS_BASE;IS_PLUGIN;IS_MONO] <src_files>...)
# first parameter is the name of the library
# the second parameter is the type of library, empty for a UI library
# all additional parameters are source files for the library
macro(wx_add_library name)
    cmake_parse_arguments(wxADD_LIBRARY "IS_BASE;IS_PLUGIN;IS_MONO" "" "" ${ARGN})
    set(src_files ${wxADD_LIBRARY_UNPARSED_ARGUMENTS})

    list(APPEND wxLIB_TARGETS ${name})
    set(wxLIB_TARGETS ${wxLIB_TARGETS} PARENT_SCOPE)

    if(wxBUILD_MONOLITHIC AND NOT wxADD_LIBRARY_IS_MONO)
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
        wx_set_target_properties(${name} ${ARGN})
        set_target_properties(${name} PROPERTIES PROJECT_LABEL ${name_short})

        # Setup install
        set(runtime_dir "lib")
        if(WIN32 AND NOT WIN32_MSVC_NAMING)
            # configure puts the .dll in the bin directory
            set(runtime_dir "bin")
        endif()
        wx_install(TARGETS ${name}
            EXPORT wxWidgetsTargets
            LIBRARY DESTINATION "lib${GEN_EXPR_DIR}${wxPLATFORM_LIB_DIR}"
            ARCHIVE DESTINATION "lib${GEN_EXPR_DIR}${wxPLATFORM_LIB_DIR}"
            RUNTIME DESTINATION "${runtime_dir}${GEN_EXPR_DIR}${wxPLATFORM_LIB_DIR}"
            BUNDLE DESTINATION Applications/wxWidgets
            )
        wx_target_enable_precomp(${name} "${wxSOURCE_DIR}/include/wx/wxprec.h")
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
    if(TARGET ${name})
        if(wxBUILD_MONOLITHIC)
            target_link_libraries(${name} PUBLIC wxmono)
        else()
            target_link_libraries(${name};PRIVATE;${ARGN})
        endif()
    endif()
endmacro()

# wx_lib_include_directories(name files)
# Forwards everything to target_include_directories() except for monolithic
# build where it collects all include paths for linking with the mono lib
macro(wx_lib_include_directories name)
    if(wxBUILD_MONOLITHIC)
        list(APPEND wxMONO_INCLUDE_DIRS ${ARGN})
        set(wxMONO_INCLUDE_DIRS ${wxMONO_INCLUDE_DIRS} PARENT_SCOPE)
    else()
        target_include_directories(${name} BEFORE PRIVATE ${ARGN})
    endif()
endmacro()

# wx_lib_compile_definitions(name defs)
# Forwards everything to target_compile_definitions() except for monolithic
# build where it collects all definitions for linking with the mono lib
macro(wx_lib_compile_definitions name)
    if(wxBUILD_MONOLITHIC)
        list(APPEND wxMONO_DEFINITIONS ${ARGN})
        set(wxMONO_DEFINITIONS ${wxMONO_DEFINITIONS} PARENT_SCOPE)
    else()
        target_compile_definitions(${name} PRIVATE ${ARGN})
    endif()
endmacro()

# wx_add_dependencies(name dep)
# Forwards everything to add_dependencies() except for monolithic
# build where it collects all dependencies for linking with the mono lib
macro(wx_add_dependencies name)
    if(wxBUILD_MONOLITHIC)
        list(APPEND wxMONO_DEPENDENCIES ${ARGN})
        set(wxMONO_DEPENDENCIES ${wxMONO_DEPENDENCIES} PARENT_SCOPE)
    else()
        add_dependencies(${name} ${ARGN})
    endif()
endmacro()

# Set common properties for a builtin third party library
function(wx_set_builtin_target_properties target_name)
    set(lib_unicode)
    if(target_name STREQUAL "wxregex")
        set(lib_unicode "u")
    endif()

    set(lib_rls)
    set(lib_dbg)
    if(WIN32_MSVC_NAMING)
        set(lib_dbg "d")
    endif()

    wx_get_flavour(lib_flavour "_")

    set(lib_version)
    if(NOT WIN32_MSVC_NAMING)
        set(lib_version "-${wxMAJOR_VERSION}.${wxMINOR_VERSION}")
    endif()

    set_target_properties(${target_name} PROPERTIES
        OUTPUT_NAME       "${target_name}${lib_unicode}${lib_rls}${lib_flavour}${lib_version}"
        OUTPUT_NAME_DEBUG "${target_name}${lib_unicode}${lib_dbg}${lib_flavour}${lib_version}"
    )

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

    target_include_directories(${target_name} BEFORE PRIVATE ${wxSETUP_HEADER_PATH})

    set_target_properties(${target_name} PROPERTIES FOLDER "Third Party Libraries")

    if(wxBUILD_SHARED OR wxBUILD_PIC)
        set_target_properties(${target_name} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
    endif()

    wx_set_common_target_properties(${target_name} DEFAULT_WARNINGS)
    if(NOT wxBUILD_SHARED)
        wx_install(TARGETS ${name} EXPORT wxWidgetsTargets ARCHIVE DESTINATION "lib${GEN_EXPR_DIR}${wxPLATFORM_LIB_DIR}")
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
    set_target_properties(${name} PROPERTIES PROJECT_LABEL ${name_short})
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
        if(NOT ${lib_name}_FOUND)
            wx_option_force_value(${var_name} builtin)
        endif()
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

# Add sample, test, demo or benchmark
# wx_add(<name> <group> [CONSOLE|CONSOLE_GUI|DLL] [IMPORTANT] [SRC_FILES...]
#    [LIBRARIES ...] [NAME target_name] [FOLDER folder]
#    [DATA ...] [DEFINITIONS ...] [RES ...] [PLIST ...)
# name default target name
# group can be Samples, Tests, Demos or Benchmarks
# first parameter may be CONSOLE to indicate a console application or DLL to indicate a shared library
# or CONSOLE_GUI to indicate a console application that uses gui libraries
# all following parameters are src files for the executable
#
# Optionally:
#   IMPORTANT (samples only) does not require wxBUILD_SAMPLES=ALL
#   LIBRARIES followed by required libraries
#   NAME alternative target_name
#   FOLDER subfolder in IDE
#   DATA followed by required data files. Use a colon to separate different source and dest paths
#   DEFINITIONS list of definitions for the target
#   RES followed by WIN32 .rc files
#   PLIST followed by macOS Info.plist.in file
#
# Additionally the following variables may be set before calling wx_add_sample:
# wxSAMPLE_SUBDIR subdirectory in the samples/ folder to use as base
# wxSAMPLE_FOLDER IDE sub folder to be used for the samples

function(wx_add_sample name)
    wx_add(${name} "Samples" ${ARGN})
endfunction()

function(wx_add_test name)
    wx_add(${name} "Tests" ${ARGN})
endfunction()

function(wx_add_demo name)
    wx_add(${name} "Demos" ${ARGN})
endfunction()

function(wx_add_benchmark name)
    wx_add(${name} "Benchmarks" ${ARGN})
endfunction()

function(wx_add name group)
    cmake_parse_arguments(APP
        "CONSOLE;CONSOLE_GUI;DLL;IMPORTANT"
        "NAME;FOLDER"
        "DATA;DEFINITIONS;DEPENDS;LIBRARIES;RES;PLIST"
        ${ARGN}
        )

    if(APP_NAME)
        set(target_name ${APP_NAME})
    else()
        set(target_name ${name})
    endif()

    if(group STREQUAL Samples)
        if(NOT APP_IMPORTANT AND NOT wxBUILD_SAMPLES STREQUAL "ALL")
            return()
        endif()
        set(SUB_DIR "samples/${wxSAMPLE_SUBDIR}${name}")
        set(DEFAULT_RC_FILE "samples/sample.rc")
    elseif(group STREQUAL Tests)
        if(NOT APP_CONSOLE AND NOT wxBUILD_TESTS STREQUAL "ALL")
            return()
        endif()
        set(SUB_DIR "tests")
        set(DEFAULT_RC_FILE "samples/sample.rc")
    elseif(group STREQUAL Demos)
        set(SUB_DIR "demos/${name}")
        set(DEFAULT_RC_FILE "demos/${name}/${target_name}.rc")
    elseif(group STREQUAL Benchmarks)
        set(SUB_DIR "tests/benchmarks")
        set(DEFAULT_RC_FILE "samples/sample.rc")
    else()
        message(WARNING "Unknown group \"${group}\"")
        return()
    endif()

    foreach(depend ${APP_DEPENDS})
        if(NOT ${depend})
            return()
        endif()
    endforeach()

    # Only build GUI applications with wxUSE_GUI=1
    if(NOT wxUSE_GUI AND NOT APP_CONSOLE)
        return()
    endif()

    if(APP_UNPARSED_ARGUMENTS)
        wx_list_add_prefix(src_files
            "${wxSOURCE_DIR}/${SUB_DIR}/"
            ${APP_UNPARSED_ARGUMENTS})
    else()
        # If no source files have been specified use default src name
        set(src_files ${wxSOURCE_DIR}/${SUB_DIR}/${name}.cpp)
    endif()

    if(WIN32)
        if(APP_RES)
            foreach(res ${APP_RES})
                list(APPEND src_files ${wxSOURCE_DIR}/${SUB_DIR}/${res})
            endforeach()
        else()
            # Include default resource file
            list(APPEND src_files ${wxSOURCE_DIR}/${DEFAULT_RC_FILE})
        endif()
    elseif(APPLE AND NOT IPHONE)
        list(APPEND src_files ${wxSOURCE_DIR}/src/osx/carbon/wxmac.icns)
    endif()

    if(APP_DLL)
        add_library(${target_name} SHARED ${src_files})
    else()
        if(APP_CONSOLE OR APP_CONSOLE_GUI)
            set(exe_type)
        else()
            set(exe_type WIN32 MACOSX_BUNDLE)
        endif()

        if (WXMSW AND DEFINED wxUSE_DPI_AWARE_MANIFEST)
            set(wxUSE_DPI_AWARE_MANIFEST_VALUE 0)
            if (${wxUSE_DPI_AWARE_MANIFEST} MATCHES "system")
                set(wxUSE_DPI_AWARE_MANIFEST_VALUE 1)
                list(APPEND src_files "${wxSOURCE_DIR}/include/wx/msw/wx_dpi_aware.manifest")
            elseif(${wxUSE_DPI_AWARE_MANIFEST} MATCHES "per-monitor")
                set(wxUSE_DPI_AWARE_MANIFEST_VALUE 2)
                list(APPEND src_files "${wxSOURCE_DIR}/include/wx/msw/wx_dpi_aware_pmv2.manifest")
            endif()
        endif()

        add_executable(${target_name} ${exe_type} ${src_files})

        if (DEFINED wxUSE_DPI_AWARE_MANIFEST_VALUE)
            target_compile_definitions(${target_name} PRIVATE wxUSE_DPI_AWARE_MANIFEST=${wxUSE_DPI_AWARE_MANIFEST_VALUE})
        endif()
    endif()

    # All applications use at least the base library other libraries
    # will have to be added with wx_link_sample_libraries()
    wx_exe_link_libraries(${target_name} wxbase)
    if(NOT APP_CONSOLE)
        # UI applications always require core
        wx_exe_link_libraries(${target_name} wxcore)
    else()
        target_compile_definitions(${target_name} PRIVATE wxUSE_GUI=0 wxUSE_BASE=1)
    endif()
    if(APP_LIBRARIES)
        wx_exe_link_libraries(${target_name} ${APP_LIBRARIES})
    endif()
    if(APP_DEFINITIONS)
        target_compile_definitions(${target_name} PRIVATE ${APP_DEFINITIONS})
    endif()

    if(group STREQUAL Samples)
        target_include_directories(${target_name} PRIVATE ${wxSOURCE_DIR}/samples)
    elseif(group STREQUAL Tests)
        target_include_directories(${target_name} PRIVATE ${wxSOURCE_DIR}/tests)
        target_include_directories(${target_name} PRIVATE ${wxSOURCE_DIR}/3rdparty/catch/single_include)
        target_include_directories(${target_name} PRIVATE ${wxTOOLKIT_INCLUDE_DIRS})
    endif()

    if(APP_DATA)
        # TODO: handle data files differently for OS X bundles
        # Copy data files to output directory
        foreach(data_src ${APP_DATA})
            string(FIND ${data_src} ":" HAS_COLON)
            if(${HAS_COLON} GREATER -1)
                MATH(EXPR DEST_INDEX "${HAS_COLON}+1")
                string(SUBSTRING ${data_src} ${DEST_INDEX} -1 data_dst)
                string(SUBSTRING ${data_src} 0 ${HAS_COLON} data_src)
            else()
                set(data_dst ${data_src})
            endif()

            list(APPEND cmds COMMAND ${CMAKE_COMMAND}
                -E copy ${wxSOURCE_DIR}/${SUB_DIR}/${data_src}
                ${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}/${data_dst})
        endforeach()
        add_custom_command(
            TARGET ${target_name} ${cmds}
            COMMENT "Copying ${target_name} data files...")
    endif()

    if(APPLE)
        if(NOT IPHONE)
            set(PLIST_FILE "${wxSOURCE_DIR}/src/osx/carbon/Info.plist.in")
            if(APP_PLIST)
                set(PLIST_FILE "${wxSOURCE_DIR}/${SUB_DIR}/${APP_PLIST}")
            endif()
            set_target_properties(${target_name} PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST "${PLIST_FILE}"
                RESOURCE "${wxSOURCE_DIR}/src/osx/carbon/wxmac.icns")
        endif()
        set_target_properties(${target_name} PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER "org.wxwidgets.${target_name}"
            MACOSX_BUNDLE_EXECUTABLE_NAME "${target_name}"
            MACOSX_BUNDLE_BUNDLE_NAME "${target_name}"
            MACOSX_BUNDLE_COPYRIGHT "Copyright ${wxCOPYRIGHT}"
            MACOSX_BUNDLE_BUNDLE_VERSION "${wxVERSION}"
            MACOSX_BUNDLE_INFO_STRING "${target_name} version ${wxVERSION}, (c) ${wxCOPYRIGHT}"
            MACOSX_BUNDLE_LONG_VERSION_STRING "${wxVERSION}, (c) ${wxCOPYRIGHT}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${wxMAJOR_VERSION}.${wxMINOR_VERSION}"
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "org.wxwidgets.${target_name}"
            )
    endif()

    if(APP_FOLDER)
        set(APP_FOLDER ${group}/${APP_FOLDER})
    elseif(wxSAMPLE_FOLDER)
        set(APP_FOLDER ${group}/${wxSAMPLE_FOLDER})
    else()
        set(APP_FOLDER ${group})
    endif()
    wx_set_common_target_properties(${target_name})
    wx_target_enable_precomp(${target_name} "${wxSOURCE_DIR}/include/wx/wxprec.h")
    set_target_properties(${target_name} PROPERTIES
        FOLDER ${APP_FOLDER}
        )
    set_target_properties(${target_name} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}"
        )

    if(group STREQUAL Tests)
        add_test(NAME ${target_name}
            COMMAND ${target_name}
            WORKING_DIRECTORY "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
    endif()
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

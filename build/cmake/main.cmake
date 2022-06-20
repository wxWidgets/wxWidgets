#############################################################################
# Name:        build/cmake/main.cmake
# Purpose:     Main CMake file
# Author:      Tobias Taschner
# Created:     2016-09-20
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

list(APPEND CMAKE_MODULE_PATH "${wxSOURCE_DIR}/build/cmake/modules")

include(build/cmake/files.cmake)            # Files list
include(build/cmake/source_groups.cmake)    # Source group definitions
include(build/cmake/functions.cmake)        # wxWidgets functions
include(build/cmake/toolkit.cmake)          # Platform/toolkit settings
include(build/cmake/options.cmake)          # User options
include(build/cmake/init.cmake)             # Init various global build vars
include(build/cmake/pch.cmake)              # Precompiled header support

add_subdirectory(build/cmake/lib libs)
add_subdirectory(build/cmake/utils utils)

if(wxBUILD_SAMPLES)
    add_subdirectory(build/cmake/samples samples)
endif()

if(wxBUILD_TESTS)
    enable_testing()
    add_subdirectory(build/cmake/tests tests)
endif()

if(wxBUILD_DEMOS)
    add_subdirectory(build/cmake/demos demos)
endif()

if(wxBUILD_BENCHMARKS)
    add_subdirectory(build/cmake/benchmarks benchmarks)
endif()

if(NOT wxBUILD_CUSTOM_SETUP_HEADER_PATH)
    # Write setup.h after all variables are available
    include(build/cmake/setup.cmake)
endif()

if(WIN32_MSVC_NAMING)
    include(build/cmake/build_cfg.cmake)
endif()

if(NOT MSVC)
    # Write wx-config
    include(build/cmake/config.cmake)
endif()

# Install target support
include(build/cmake/install.cmake)

# Determine minimum required OS at runtime
set(wxREQUIRED_OS_DESC "${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
if(MSVC OR MINGW OR CYGWIN)
    # Determine based on used toolkit
    if(MINGW OR CYGWIN OR (MSVC_VERSION LESS 1700) OR (CMAKE_VS_PLATFORM_TOOLSET MATCHES "_xp$") )
        # Visual Studio < 2012 and MinGW always create XP compatible binaries
        # XP Toolset is required since VS 2012
        set(wxREQUIRED_OS_DESC "Windows XP / Windows Server 2003")
    else()
        set(wxREQUIRED_OS_DESC "Windows Vista / Windows Server 2008")
    endif()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        wx_string_append(wxREQUIRED_OS_DESC " (x64 Edition)")
    endif()
elseif(APPLE AND NOT IPHONE)
    if(DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
        set(wxREQUIRED_OS_DESC "macOS ${CMAKE_OSX_DEPLOYMENT_TARGET}")
    endif()
endif()

# Print configuration summary
wx_print_thirdparty_library_summary()

message(STATUS "Configured wxWidgets ${wxVERSION} for ${CMAKE_SYSTEM}
    Min OS Version required at runtime:                ${wxREQUIRED_OS_DESC}
    Which GUI toolkit should wxWidgets use?            ${wxBUILD_TOOLKIT} ${wxTOOLKIT_VERSION}
    Should wxWidgets be compiled into single library?  ${wxBUILD_MONOLITHIC}
    Should wxWidgets be linked as a shared library?    ${wxBUILD_SHARED}
    Should wxWidgets support Unicode?                  ${wxUSE_UNICODE}
    What wxWidgets compatibility level should be used? ${wxBUILD_COMPATIBILITY}"
    )

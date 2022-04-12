#############################################################################
# Name:        CMakeLists.txt
# Purpose:     Primary CMake for wxWidgets
# Author:      Tobias Taschner
# Created:     2016-09-20
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

cmake_minimum_required(VERSION 2.8.12)

if(NOT CMAKE_CONFIGURATION_TYPES)
    get_property(HAVE_MULTI_CONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    # Set default configuration types for multi-config generators
    if(HAVE_MULTI_CONFIG_GENERATOR)
        set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    endif()
endif()

# https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(default_build_type "Debug")
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# This block, particularly the versions used, should be kept in sync with
# samples/minimal/CMakeLists.txt.
if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    # If no deployment target has been set default to the minimum supported
    # OS version (this has to be set before the first project() call)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "iOS Deployment Target")
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10 CACHE STRING "macOS Deployment Target")
    endif()
endif()

include(build/cmake/policies.cmake NO_POLICY_SCOPE)

# Initialize variables for quick access to wx root dir in sub dirs
set(wxSOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(wxBINARY_DIR ${CMAKE_BINARY_DIR})
set(wxOUTPUT_DIR ${wxBINARY_DIR}/lib)

# parse the version number from wx/version.h and include in wxMAJOR_VERSION and wxMINOR_VERSION
file(READ "${wxSOURCE_DIR}/include/wx/version.h" WX_VERSION_H_CONTENTS)
string(REGEX MATCH "wxMAJOR_VERSION[ \t]+([0-9]+)"
    wxMAJOR_VERSION ${WX_VERSION_H_CONTENTS})
string (REGEX MATCH "([0-9]+)"
    wxMAJOR_VERSION ${wxMAJOR_VERSION})
string(REGEX MATCH "wxMINOR_VERSION[ \t]+([0-9]+)"
    wxMINOR_VERSION ${WX_VERSION_H_CONTENTS})
string (REGEX MATCH "([0-9]+)"
    wxMINOR_VERSION ${wxMINOR_VERSION})
string(REGEX MATCH "wxRELEASE_NUMBER[ \t]+([0-9]+)"
    wxRELEASE_NUMBER ${WX_VERSION_H_CONTENTS})
string (REGEX MATCH "([0-9]+)"
    wxRELEASE_NUMBER ${wxRELEASE_NUMBER})

# Determine if current version is a "Development" release
math(EXPR rel_dev "${wxMINOR_VERSION} % 2")
if(rel_dev)
    set(wxVERSION_IS_DEV TRUE)
else()
    set(wxVERSION_IS_DEV FALSE)
endif()

# parse the .so version from build/bakefiles/version.bkl
file(READ "${wxSOURCE_DIR}/build/bakefiles/version.bkl" WX_VERSION_BKL_CONTENTS)
string(REGEX MATCH "WX_CURRENT.>([0-9]+)"
    WX_CURRENT ${WX_VERSION_BKL_CONTENTS})
string(REGEX MATCH "([0-9]+)"
    WX_CURRENT ${WX_CURRENT})
string(REGEX MATCH "WX_REVISION.>([0-9]+)"
    WX_REVISION ${WX_VERSION_BKL_CONTENTS})
string(REGEX MATCH "([0-9]+)"
    WX_REVISION ${WX_REVISION})
string(REGEX MATCH "WX_AGE.>([0-9]+)"
    WX_AGE ${WX_VERSION_BKL_CONTENTS})
string(REGEX MATCH "([0-9]+)"
    WX_AGE ${WX_AGE})
math(EXPR wxSOVERSION_MAJOR "${WX_CURRENT} - ${WX_AGE}")
set(wxSOVERSION ${wxSOVERSION_MAJOR}.${WX_AGE}.${WX_REVISION})

set(wxVERSION ${wxMAJOR_VERSION}.${wxMINOR_VERSION}.${wxRELEASE_NUMBER})
set(wxCOPYRIGHT "2002-2022 wxWidgets")

set(wxLANGUAGES C CXX)
if(APPLE AND CMAKE_VERSION GREATER_EQUAL "3.16")
    list(APPEND wxLANGUAGES OBJCXX)
endif()

project(wxWidgets VERSION ${wxVERSION} LANGUAGES ${wxLANGUAGES})

include(build/cmake/main.cmake)

# Set the default startup project for Visual Studio
if(wxBUILD_SAMPLES AND wxUSE_GUI)
    set_directory_properties(PROPERTIES VS_STARTUP_PROJECT minimal)
endif()

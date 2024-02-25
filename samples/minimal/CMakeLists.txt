#############################################################################
# Name:        samples/minimal/CMakeListst.txt
# Purpose:     Sample CMake file to show usage of cmake for wxWidgets users
# Author:      Tobias Taschner
# Created:     2016-10-23
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

#
# This file is just a sample to show using cmake from an application
# If you want to build the minimal and other samples please use the
# wxBUILD_SAMPLES option when using cmake on the library
#

# Declare the minimum required CMake version
cmake_minimum_required(VERSION 3.5)

if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    # If no deployment target has been set default to the minimum supported
    # OS version (this has to be set before the first project() call)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "iOS Deployment Target")
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10 CACHE STRING "macOS Deployment Target")
    endif()
endif()

# Name the project
project(minimal)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Request the required wxWidgets libs
find_package(wxWidgets 3.3 COMPONENTS core base REQUIRED CONFIG)

# Define a variable containing a list of source files for the project
set(SRC_FILES
    minimal.cpp
)

if(WIN32)
    # Include a RC file for windows
    list(APPEND SRC_FILES ../sample.rc)
elseif(APPLE)
    # Add an icon for the apple .app file
    list(APPEND SRC_FILES ../../src/osx/carbon/wxmac.icns)
endif()

# Define the build target for the executable
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SRC_FILES})
# Link executable to the wxWidgets target
target_link_libraries(${PROJECT_NAME} wxWidgets::wxWidgets)

if(MSVC)
    # Use manifest from wx.rc
    set_target_properties(${PROJECT_NAME} PROPERTIES
        COMPILE_FLAGS "/DwxUSE_RC_MANIFEST"
        LINK_FLAGS "/MANIFEST:NO"
    )
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RESOURCE "../../src/osx/carbon/wxmac.icns"
        MACOSX_BUNDLE_ICON_FILE wxmac.icns
        MACOSX_BUNDLE_COPYRIGHT "Copyright wxWidgets"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.wxwidgets.minimal"
    )
endif()

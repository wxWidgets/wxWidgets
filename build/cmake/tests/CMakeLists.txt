#############################################################################
# Name:        build/cmake/tests/CMakeLists.txt
# Purpose:     Build test executables
# Author:      Tobias Taschner
# Created:     2016-09-24
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# Find CPPUnit
wx_add_thirdparty_library(wxUSE_CPPUNIT cppunit 
    "use cppunit (Used for unit tests)" DEFAULT sys)

if(wxUSE_CPPUNIT STREQUAL "builtin")
    # Build our own private copy if it could not be found in the system
    message(STATUS "Could not find cppunit. Downloading and building local copy")
    set(CPPUNIT_URL http://dev-www.libreoffice.org/src/cppunit-1.13.2.tar.gz)
    set(CPPUNIT_MD5 d1c6bdd5a76c66d2c38331e2d287bc01)
    add_library(cppunit STATIC IMPORTED)
    if(MSVC AND DEFINED CMAKE_VS_MSBUILD_COMMAND)
        # Build with VS 2010+
        if(CMAKE_VS_PLATFORM_TOOLSET)
            set(build_param_toolset /p:PlatformToolset=${CMAKE_VS_PLATFORM_TOOLSET})
            if(MSVC_VERSION EQUAL 1700)
                # VS11 requires an additional parameter to build VS10 project files
                set(build_param_toolset ${build_param_toolset} /p:VisualStudioVersion=11.0)
            endif()
        else()
            # Maybe empty with VS10, but that should be fine
            # because the vcxproj is VS10
            set(build_param_toolset)
        endif()
        ExternalProject_Add(cppunitlib
            URL ${CPPUNIT_URL} 
            URL_MD5 ${CPPUNIT_MD5}
            CONFIGURE_COMMAND ""
            BUILD_IN_SOURCE 1
            BUILD_COMMAND
                ${CMAKE_VS_MSBUILD_COMMAND}
                    src/cppunit/cppunit.vcxproj
                    /p:Configuration=$<CONFIGURATION>
                    /p:Platform=${CMAKE_VS_PLATFORM_NAME}
                    ${build_param_toolset}
            INSTALL_COMMAND
                ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/lib/cppunit$<$<CONFIG:Debug>:d>.lib <INSTALL_DIR>
            COMMAND
                ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/include <INSTALL_DIR>/include 
            )
        ExternalProject_Get_Property(cppunitlib INSTALL_DIR)
        set_target_properties(cppunit PROPERTIES
            IMPORTED_LOCATION "${INSTALL_DIR}/cppunit.lib"
            IMPORTED_LOCATION_DEBUG "${INSTALL_DIR}/cppunitd.lib"
            )
    elseif(UNIX)
        # TODO: forward CC and CCFLAGS
        ExternalProject_Add(cppunitlib
            URL ${CPPUNIT_URL}
            URL_MD5 ${CPPUNIT_MD5}
            CONFIGURE_COMMAND <SOURCE_DIR>/configure
                --prefix=<INSTALL_DIR>
                --disable-shared
                --disable-doxygen
                --disable-html-docs
            BUILD_COMMAND make
            INSTALL_COMMAND make install
            )
        ExternalProject_Get_Property(cppunitlib INSTALL_DIR)
        set_target_properties(cppunit PROPERTIES IMPORTED_LOCATION
            "${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}cppunit${CMAKE_STATIC_LIBRARY_SUFFIX}")
    else()
        set(wxUSE_CPPUNIT OFF)
    endif()

    set(CPPUNIT_INCLUDE_DIR "${INSTALL_DIR}/include")
    set(CPPUNIT_LIBRARIES cppunit)
    add_dependencies(cppunit cppunitlib)
elseif(wxUSE_CPPUNIT)
    find_package(cppunit REQUIRED)
endif()

if(NOT wxUSE_CPPUNIT)
    message(FATAL_ERROR "cppunit is required for tests. Please install cppunit or set wxBUILD_TESTS=OFF")
endif()

add_subdirectory(base)

# Build GUI tests
if(wxUSE_GUI AND wxBUILD_TESTS STREQUAL "ALL")

add_subdirectory(drawing)
add_subdirectory(gui)

endif()

# Propagate variable(s) to parent scope
set(wxTHIRD_PARTY_LIBRARIES ${wxTHIRD_PARTY_LIBRARIES} PARENT_SCOPE)

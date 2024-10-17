#############################################################################
# Name:        build/cmake/policies.cmake
# Purpose:     CMake policies for wxWidgets
# Author:      Tobias Taschner
# Created:     2016-10-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# Please keep the policies in the order of their numbers.

if(POLICY CMP0025)
    # Compiler id for Apple Clang is now AppleClang.
    cmake_policy(SET CMP0025 NEW)
endif()

if(POLICY CMP0028)
    # Double colon in target name means ALIAS or IMPORTED target.
    cmake_policy(SET CMP0028 NEW)
endif()

if(POLICY CMP0038)
    # Targets may not link directly to themselves.
    cmake_policy(SET CMP0038 NEW)
endif()

if(POLICY CMP0042)
    # MACOSX_RPATH is enabled by default.
    cmake_policy(SET CMP0042 NEW)
endif()

if(POLICY CMP0045)
    # Error on non-existent target in get_target_property.
    cmake_policy(SET CMP0045 NEW)
endif()

if(POLICY CMP0046)
    # Error on non-existent dependency in add_dependencies.
    cmake_policy(SET CMP0046 NEW)
endif()

if(POLICY CMP0048)
    # The project() command manages VERSION variables.
    cmake_policy(SET CMP0048 NEW)
endif()

if(POLICY CMP0049)
    # Do not expand variables in target source entries.
    cmake_policy(SET CMP0049 NEW)
endif()

if(POLICY CMP0053)
    # Simplify variable reference and escape sequence evaluation.
    cmake_policy(SET CMP0053 NEW)
endif()

if(POLICY CMP0054)
    # Only interpret if() arguments as variables or keywords when unquoted.
    cmake_policy(SET CMP0054 NEW)
endif()

if(POLICY CMP0057)
    # Support new if() IN_LIST operator.
    cmake_policy(SET CMP0057 NEW)
endif()

if(POLICY CMP0060)
    # Link libraries by full path even in implicit directories.
    cmake_policy(SET CMP0060 NEW)
endif()

if(POLICY CMP0067)
    # Honor language standard in try_compile() source-file signature.
    cmake_policy(SET CMP0067 NEW)
endif()

if(POLICY CMP0072)
    # FindOpenGL prefers GLVND by default when available.
    cmake_policy(SET CMP0072 NEW)
endif()

if(POLICY CMP0079)
    # target_link_libraries() allows use with targets in other directories.
    cmake_policy(SET CMP0079 NEW)
endif()

if(POLICY CMP0092)
    # MSVC warning flags are not in CMAKE_<LANG>_FLAGS by default.
    cmake_policy(SET CMP0092 NEW)
endif()

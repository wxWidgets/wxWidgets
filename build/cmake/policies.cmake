#############################################################################
# Name:        CMakeLists.txt
# Purpose:     CMake policies for wxWidgets
# Author:      Tobias Taschner
# Created:     2016-10-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# Please keep the policies in the order of their numbers.

if(POLICY CMP0025)
    # Compiler id for Apple Clang is now AppleClang
    cmake_policy(SET CMP0025 NEW)
endif()

if(POLICY CMP0038)
    # targets may not link directly to themselves
    cmake_policy(SET CMP0038 NEW)
endif()

if(POLICY CMP0042)
    # MACOSX_RPATH is enabled by default.
    cmake_policy(SET CMP0042 NEW)
endif()

if(POLICY CMP0045)
    # error on non-existent target in get_target_property
    cmake_policy(SET CMP0045 NEW)
endif()

if(POLICY CMP0046)
    # error on non-existent dependency in add_dependencies
    cmake_policy(SET CMP0046 NEW)
endif()

if(POLICY CMP0049)
    # do not expand variables in target source entries
    cmake_policy(SET CMP0049 NEW)
endif()

if(POLICY CMP0053)
    # simplify variable reference and escape sequence evaluation
    cmake_policy(SET CMP0053 NEW)
endif()

if(POLICY CMP0054)
    # only interpret if() arguments as variables or keywords when unquoted
    cmake_policy(SET CMP0054 NEW)
endif()

if(POLICY CMP0067)
    # Honor language standard in try_compile() source-file signature.
    cmake_policy(SET CMP0067 NEW)
endif()

if(POLICY CMP0072)
    # FindOpenGL prefers GLVND by default when available.
    cmake_policy(SET CMP0072 NEW)
endif()

# - Try to find GTK+ 4
# Once done, this will define
#
# GTK4_FOUND - system has GTK+ 4.
# GTK4_INCLUDE_DIRS - the GTK+ 4. include directories
# GTK4_LIBRARIES - link these to use GTK+ 4.
#
# Copyright (C) 2012 Raphael Kubo da Costa <rakuco@webkit.org>
# Copyright (C) 2013 Igalia S.L.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND ITS CONTRIBUTORS ``AS
# IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ITS
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
find_package(PkgConfig)
pkg_check_modules(GTK4 QUIET gtk4)
set(VERSION_OK TRUE)
if (GTK4_VERSION)
if (GTK4_FIND_VERSION_EXACT)
if (NOT("${GTK4_FIND_VERSION}" VERSION_EQUAL "${GTK4_VERSION}"))
set(VERSION_OK FALSE)
endif ()
else ()
if ("${GTK4_VERSION}" VERSION_LESS "${GTK4_FIND_VERSION}")
set(VERSION_OK FALSE)
endif ()
endif ()
endif ()
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GTK4 DEFAULT_MSG GTK4_INCLUDE_DIRS GTK4_LIBRARIES VERSION_OK)

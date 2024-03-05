# - Try to find GStreamer and its plugins
# Once done, this will define
#
#  GSTREAMER_FOUND - system has GStreamer
#  GSTREAMER_INCLUDE_DIRS - the GStreamer include directories
#  GSTREAMER_LIBRARIES - link these to use GStreamer
#
# Additionally, gstreamer-base is always looked for and required, and
# the following related variables are defined:
#
#  GSTREAMER_BASE_INCLUDE_DIRS - gstreamer-base's include directory
#  GSTREAMER_BASE_LIBRARIES - link to these to use gstreamer-base
#
# Optionally, the COMPONENTS keyword can be passed to find_package()
# and GStreamer plugins can be looked for.  Currently, the following
# plugins can be searched, and they define the following variables if
# found:
#
#  gstreamer-app:        GSTREAMER_APP_INCLUDE_DIRS and GSTREAMER_APP_LIBRARIES
#  gstreamer-audio:      GSTREAMER_AUDIO_INCLUDE_DIRS and GSTREAMER_AUDIO_LIBRARIES
#  gstreamer-fft:        GSTREAMER_FFT_INCLUDE_DIRS and GSTREAMER_FFT_LIBRARIES
#  gstreamer-interfaces: GSTREAMER_INTERFACES_INCLUDE_DIRS and GSTREAMER_INTERFACES_LIBRARIES
#  gstreamer-pbutils:    GSTREAMER_PBUTILS_INCLUDE_DIRS and GSTREAMER_PBUTILS_LIBRARIES
#  gstreamer-video:      GSTREAMER_VIDEO_INCLUDE_DIRS and GSTREAMER_VIDEO_LIBRARIES
#  gstreamer-player:     GSTREAMER_PLAYER_INCLUDE_DIRS and GSTREAMER_PLAYER_LIBRARIES
#
# Copyright (C) 2012 Raphael Kubo da Costa <rakuco@webkit.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
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

# Determine the version in the library name, default is 1.0
set(GST_LIB_VERSION 1.0)
if(DEFINED GSTREAMER_FIND_VERSION AND GSTREAMER_FIND_VERSION VERSION_LESS 1.0)
    set(GST_LIB_VERSION 0.10)
endif()

# Helper macro to find a GStreamer plugin (or GStreamer itself)
#   _component_prefix is prepended to the _INCLUDE_DIRS and _LIBRARIES variables (eg. "GSTREAMER_AUDIO")
#   _pkgconfig_name is the component's pkg-config name (eg. "gstreamer", or "gstreamer-video").
#   _header is the component's header, relative to the gstreamer-${GST_LIB_VERSION} directory (eg. "gst/gst.h").
#   _library is the component's library name (eg. "gstreamer" or "gstvideo")
macro(FIND_GSTREAMER_COMPONENT _component_prefix _pkgconfig_name _header _library)
    pkg_check_modules(PC_${_component_prefix} QUIET ${_pkgconfig_name}-${GST_LIB_VERSION})

    find_path(${_component_prefix}_INCLUDE_DIRS
        NAMES ${_header}
        HINTS ${PC_${_component_prefix}_INCLUDE_DIRS} ${PC_${_component_prefix}_INCLUDEDIR}
        PATH_SUFFIXES gstreamer-${GST_LIB_VERSION}
    )

    find_library(${_component_prefix}_LIBRARIES
        NAMES ${_library}-${GST_LIB_VERSION}
        HINTS ${PC_${_component_prefix}_LIBRARY_DIRS} ${PC_${_component_prefix}_LIBDIR}
    )

    mark_as_advanced(${_component_prefix}_INCLUDE_DIRS ${_component_prefix}_LIBRARIES)
endmacro()

# ------------------------
# 1. Find GStreamer itself
# ------------------------

# 1.1. Find headers and libraries
FIND_GSTREAMER_COMPONENT(GSTREAMER gstreamer gst/gst.h gstreamer)
FIND_GSTREAMER_COMPONENT(GSTREAMER_BASE gstreamer-base gst/gst.h gstbase)

# 1.2. Check GStreamer version
if (GSTREAMER_INCLUDE_DIRS)
    if (EXISTS "${GSTREAMER_INCLUDE_DIRS}/gst/gstversion.h")
        file(READ "${GSTREAMER_INCLUDE_DIRS}/gst/gstversion.h" GSTREAMER_VERSION_CONTENTS)

        string(REGEX MATCH "#define +GST_VERSION_MAJOR +\\(([0-9]+)\\)" _dummy "${GSTREAMER_VERSION_CONTENTS}")
        set(GSTREAMER_VERSION_MAJOR "${CMAKE_MATCH_1}")

        string(REGEX MATCH "#define +GST_VERSION_MINOR +\\(([0-9]+)\\)" _dummy "${GSTREAMER_VERSION_CONTENTS}")
        set(GSTREAMER_VERSION_MINOR "${CMAKE_MATCH_1}")

        string(REGEX MATCH "#define +GST_VERSION_MICRO +\\(([0-9]+)\\)" _dummy "${GSTREAMER_VERSION_CONTENTS}")
        set(GSTREAMER_VERSION_MICRO "${CMAKE_MATCH_1}")

        set(GSTREAMER_VERSION "${GSTREAMER_VERSION_MAJOR}.${GSTREAMER_VERSION_MINOR}.${GSTREAMER_VERSION_MICRO}")
    endif ()
endif ()

# -------------------------
# 2. Find GStreamer plugins
# -------------------------

FIND_GSTREAMER_COMPONENT(GSTREAMER_APP gstreamer-app gst/app/gstappsink.h gstapp)
FIND_GSTREAMER_COMPONENT(GSTREAMER_AUDIO gstreamer-audio gst/audio/audio.h gstaudio)
FIND_GSTREAMER_COMPONENT(GSTREAMER_FFT gstreamer-fft gst/fft/gstfft.h gstfft)
FIND_GSTREAMER_COMPONENT(GSTREAMER_INTERFACES gstreamer-interfaces gst/interfaces/mixer.h gstinterfaces)
FIND_GSTREAMER_COMPONENT(GSTREAMER_PBUTILS gstreamer-pbutils gst/pbutils/pbutils.h gstpbutils)
FIND_GSTREAMER_COMPONENT(GSTREAMER_VIDEO gstreamer-video gst/video/video.h gstvideo)
FIND_GSTREAMER_COMPONENT(GSTREAMER_PLAYER gstreamer-player gst/player/player.h gstplayer)

# ------------------------------------------------
# 3. Process the COMPONENTS passed to FIND_PACKAGE
# ------------------------------------------------
set(_GSTREAMER_REQUIRED_VARS GSTREAMER_VERSION GSTREAMER_INCLUDE_DIRS GSTREAMER_LIBRARIES GSTREAMER_BASE_INCLUDE_DIRS GSTREAMER_BASE_LIBRARIES)

foreach (_component ${GSTREAMER_FIND_COMPONENTS})
    set(_gst_component "GSTREAMER_${_component}")
    string(TOUPPER ${_gst_component} _UPPER_NAME)

    list(APPEND _GSTREAMER_REQUIRED_VARS ${_UPPER_NAME}_INCLUDE_DIRS ${_UPPER_NAME}_LIBRARIES)
endforeach ()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GSTREAMER DEFAULT_MSG ${_GSTREAMER_REQUIRED_VARS})

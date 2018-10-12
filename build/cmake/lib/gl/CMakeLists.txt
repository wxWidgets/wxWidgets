#############################################################################
# Name:        build/cmake/lib/gl/CMakeLists.txt
# Purpose:     CMake file for gl library
# Author:      Tobias Taschner
# Created:     2016-10-03
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

wx_append_sources(GL_FILES OPENGL_CMN)

if(WXMSW)
    wx_append_sources(GL_FILES OPENGL_MSW)
elseif(WXGTK)
    wx_append_sources(GL_FILES OPENGL_GTK)
elseif(APPLE)
    wx_append_sources(GL_FILES OPENGL_OSX_SHARED)
elseif(WXQT)
    wx_append_sources(GL_FILES OPENGL_QT)
endif()

wx_add_library(gl ${GL_FILES})
wx_lib_include_directories(gl PUBLIC ${OPENGL_INCLUDE_DIR})
wx_lib_link_libraries(gl PUBLIC ${OPENGL_LIBRARIES})

wx_finalize_lib(gl)

#############################################################################
# Name:        build/cmake/config.cmake
# Purpose:     Build wx-config script in build folder
# Author:      Tobias Taschner
# Created:     2016-10-13
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

set(wxCONFIG_DIR ${wxOUTPUT_DIR}/wx/config)
file(MAKE_DIRECTORY ${wxCONFIG_DIR})
set(TOOLCHAIN_FULLNAME ${wxBUILD_FILE_ID})

macro(wx_configure_script input output)
    set(abs_top_srcdir ${CMAKE_CURRENT_SOURCE_DIR})
    set(abs_top_builddir ${CMAKE_CURRENT_BINARY_DIR})

    configure_file(
        ${input}
        ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${output}
        ESCAPE_QUOTES @ONLY NEWLINE_STYLE UNIX)
    file(COPY
        ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${output}
        DESTINATION ${wxCONFIG_DIR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
            GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
endmacro()

function(wx_write_config_inplace)
    wx_configure_script(
        "${CMAKE_CURRENT_SOURCE_DIR}/wx-config-inplace.in"
        "inplace-${TOOLCHAIN_FULLNAME}"
        )
    execute_process(
        COMMAND
        ${CMAKE_COMMAND} -E create_symlink
        "lib/wx/config/inplace-${TOOLCHAIN_FULLNAME}"
        "${CMAKE_CURRENT_BINARY_DIR}/wx-config"
        )
endfunction()

function(wx_write_config)

    # TODO: set variables
    set(prefix ${CMAKE_INSTALL_PREFIX})
    set(exec_prefix $)
    wx_string_append(exec_prefix "{prefix}")
    set(includedir "$")
    wx_string_append(includedir "{prefix}/include")
    set(libdir "$")
    wx_string_append(libdir "{exec_prefix}/lib")
    set(bindir "$")
    wx_string_append(bindir "{exec_prefix}/bin")

    find_program(EGREP egrep)
    mark_as_advanced(EGREP)

    if(wxBUILD_MONOLITHIC)
        set(MONOLITHIC 1)
    else()
        set(MONOLITHIC 0)
    endif()
    if(wxBUILD_SHARED)
        set(SHARED 1)
    else()
        set(SHARED 0)
    endif()
    if(wxUSE_UNICODE)
        set(WX_CHARTYPE unicode)
    else()
        set(WX_CHARTYPE ansi)
    endif()
    if(CMAKE_CROSSCOMPILING)
        set(cross_compiling yes)
        set(host_alias ${CMAKE_SYSTEM_NAME})
    else()
        set(cross_compiling no)
    endif()

    set(BUILT_WX_LIBS)
    foreach(lib IN LISTS wxLIB_TARGETS)
        wx_string_append(BUILT_WX_LIBS " ${lib}")
    endforeach()
    set(STD_BASE_LIBS)
    set(STD_GUI_LIBS)

    set(WX_RELEASE ${wxMAJOR_VERSION}.${wxMINOR_VERSION})
    set(WX_VERSION ${wxVERSION})
    set(WX_SUBVERSION ${wxVERSION}.0)
    set(WX_FLAVOUR)
    set(TOOLKIT_DIR ${wxBUILD_TOOLKIT})
    set(TOOLKIT_VERSION)
    set(WIDGET_SET ${wxBUILD_WIDGETSET})
    #TODO: setting TOOLCHAIN_NAME produces change results in config folder
#    set(TOOLCHAIN_NAME)
    set(WX_LIBRARY_BASENAME_GUI)
    set(WX_LIBRARY_BASENAME_NOGUI)

    set(WXCONFIG_LIBS)
    set(EXTRALIBS_GUI)
    set(EXTRALIBS_SDL)
    set(EXTRALIBS_HTML)
    set(EXTRALIBS_STC)
    set(EXTRALIBS_WEBVIEW)
    set(EXTRALIBS_XML)
    set(EXTRALIBS_MEDIA)
    set(OPENGL_LIBS)
    set(DMALLOC_LIBS)

    set(CC ${CMAKE_C_COMPILER})
    set(CXX ${CMAKE_CXX_COMPILER})
    set(WXCONFIG_CFLAGS)
    set(WXCONFIG_LDFLAGS)
    set(WXCONFIG_CPPFLAGS)
    set(WXCONFIG_CXXFLAGS)
    set(WXCONFIG_LDFLAGS_GUI)
    set(WXCONFIG_RESFLAGS)
    set(WXCONFIG_RPATH)
    set(LDFLAGS_GL)
    set(RESCOMP)

    wx_configure_script(
        "${CMAKE_CURRENT_SOURCE_DIR}/wx-config.in"
        "${TOOLCHAIN_FULLNAME}"
        )
endfunction()

wx_write_config_inplace()
wx_write_config()

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
    # variables used in wx-config-inplace.in
    set(abs_top_srcdir ${wxSOURCE_DIR})
    set(abs_top_builddir ${wxBINARY_DIR})

    configure_file(
        ${wxSOURCE_DIR}/${input}
        ${wxBINARY_DIR}${CMAKE_FILES_DIRECTORY}/${output}
        ESCAPE_QUOTES @ONLY NEWLINE_STYLE UNIX)
    file(COPY
        ${wxBINARY_DIR}${CMAKE_FILES_DIRECTORY}/${output}
        DESTINATION ${wxCONFIG_DIR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
            GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
endmacro()

macro(wx_get_dependencies var lib)
    set(${var})
    if(TARGET wx${lib})
        get_target_property(deps wx${lib} LINK_LIBRARIES)
        foreach(dep IN LISTS deps)
            if(TARGET ${dep})
                get_target_property(dep_type ${dep} TYPE)
                if (dep_type STREQUAL "INTERFACE_LIBRARY")
                    get_target_property(dep_name ${dep} INTERFACE_OUTPUT_NAME)
                else()
                    get_target_property(dep_name ${dep} OUTPUT_NAME)
                endif()
                if(NOT dep_name)
                    # imported target
                    set(prop_suffix)
                    if (CMAKE_BUILD_TYPE)
                        string(TOUPPER "${CMAKE_BUILD_TYPE}" prop_suffix)
                        set(prop_suffix "_${prop_suffix}")
                    endif()
                    get_target_property(dep_name ${dep} LOCATION${prop_suffix})
                endif()
            else()
                # For the value like $<$<CONFIG:DEBUG>:LIB_PATH>
                # Or $<$<NOT:$<CONFIG:DEBUG>>:LIB_PATH>
                string(REGEX REPLACE "^.+>:(.+)>$" "\\1" dep_name ${dep})
                if (NOT dep_name)
                    set(dep_name ${dep})
                endif()
            endif()
            if(dep_name STREQUAL "libc.so")
                # don't include this library
            elseif(dep_name MATCHES "^-(.*)$")   # -l, -framework, -weak_framework
                wx_string_append(${var} "${dep_name} ")
            elseif(dep_name MATCHES "^lib(.*)(.so|.dylib|.tbd|.a)$")
                wx_string_append(${var} "-l${CMAKE_MATCH_1} ")
            elseif(dep_name)
                get_filename_component(abs_path ${dep_name} PATH)
                if (abs_path) # value contains path
                    wx_string_append(${var} "${dep_name} ")
                else()
                    wx_string_append(${var} "-l${dep_name} ")
                endif()
            endif()
        endforeach()
        string(STRIP ${${var}} ${var})
    endif()
endmacro()

function(wx_write_config_inplace)
    wx_configure_script(
        "wx-config-inplace.in"
        "inplace-${TOOLCHAIN_FULLNAME}"
        )
    if(WIN32_MSVC_NAMING)
        set(COPY_CMD copy)
    else()
        set(COPY_CMD create_symlink)
    endif()
    execute_process(
        COMMAND
        "${CMAKE_COMMAND}" -E ${COPY_CMD}
        "${wxBINARY_DIR}/lib/wx/config/inplace-${TOOLCHAIN_FULLNAME}"
        "${wxBINARY_DIR}/wx-config"
        )
endfunction()

function(wx_write_config)

    set(prefix ${CMAKE_INSTALL_PREFIX})
    set(exec_prefix "\${prefix}")
    set(includedir "\${prefix}/include")
    set(libdir "\${exec_prefix}/lib")
    set(bindir "\${exec_prefix}/bin")

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
    set(lib_unicode_suffix u)
    if(CMAKE_CROSSCOMPILING)
        set(cross_compiling yes)
        set(host_alias ${CMAKE_SYSTEM_NAME})
    else()
        set(cross_compiling no)
    endif()

    set(BUILT_WX_LIBS)
    set(STD_BASE_LIBS)
    set(STD_GUI_LIBS)
    set(STD_BASE_LIBS_ALL xml net base)
    set(STD_GUI_LIBS_ALL xrc html qa adv core)
    foreach(lib IN ITEMS xrc webview stc richtext ribbon propgrid aui gl media html qa adv core xml net base)
        list(FIND wxLIB_TARGETS wx${lib} hasLib)
        if (hasLib GREATER -1)
            wx_string_append(BUILT_WX_LIBS "${lib} ")
            list(FIND STD_BASE_LIBS_ALL ${lib} index)
            if (index GREATER -1)
                wx_string_append(STD_BASE_LIBS "${lib} ")
            endif()
            list(FIND STD_GUI_LIBS_ALL ${lib} index)
            if (index GREATER -1)
                wx_string_append(STD_GUI_LIBS "${lib} ")
            endif()
        endif()
    endforeach()
    string(STRIP "${BUILT_WX_LIBS}" BUILT_WX_LIBS)
    string(STRIP "${STD_BASE_LIBS}" STD_BASE_LIBS)
    string(STRIP "${STD_GUI_LIBS}" STD_GUI_LIBS)

    set(WX_RELEASE ${wxMAJOR_VERSION}.${wxMINOR_VERSION})
    set(WX_VERSION ${wxVERSION})
    set(WX_SUBVERSION ${wxVERSION}.0)
    set(WX_FLAVOUR)
    set(TOOLKIT_DIR ${wxBUILD_TOOLKIT})
    set(TOOLKIT_VERSION)
    set(WIDGET_SET ${wxBUILD_WIDGETSET})
    set(TOOLCHAIN_NAME "${TOOLKIT_DIR}${TOOLKIT_VERSION}${WIDGET_SET}${lib_unicode_suffix}-${WX_RELEASE}")
    set(WX_LIBRARY_BASENAME_GUI "wx_${TOOLKIT_DIR}${TOOLKIT_VERSION}${WIDGET_SET}${lib_unicode_suffix}")
    set(WX_LIBRARY_BASENAME_NOGUI "wx_base${lib_unicode_suffix}")

    wx_get_dependencies(WXCONFIG_LIBS base)
    wx_get_dependencies(EXTRALIBS_GUI core)
    set(EXTRALIBS_SDL) # included in core libs when SDL is enabled
    wx_get_dependencies(EXTRALIBS_HTML html)
    wx_get_dependencies(EXTRALIBS_STC stc)
    wx_get_dependencies(EXTRALIBS_WEBVIEW webview)
    wx_get_dependencies(EXTRALIBS_XML xml)
    wx_get_dependencies(EXTRALIBS_MEDIA media)
    wx_get_dependencies(OPENGL_LIBS gl)
    set(DMALLOC_LIBS)
    if(wxBUILD_MONOLITHIC)
        wx_get_dependencies(WXCONFIG_LIBS mono)
    endif()

    set(CC ${CMAKE_C_COMPILER})
    set(CXX ${CMAKE_CXX_COMPILER})
    set(WXCONFIG_CFLAGS)
    set(WXCONFIG_LDFLAGS)
    if(CMAKE_USE_PTHREADS_INIT)
        set(WXCONFIG_CFLAGS "-pthread")
        set(WXCONFIG_LDFLAGS "-pthread")
    endif()
    set(WXCONFIG_CPPFLAGS)
    if(wxBUILD_SHARED)
        wx_string_append(WXCONFIG_CPPFLAGS " -DWXUSINGDLL")
    endif()
    foreach(flag IN LISTS wxTOOLKIT_DEFINITIONS)
        wx_string_append(WXCONFIG_CPPFLAGS " -D${flag}")
    endforeach()
    if(wxBUILD_LARGEFILE_SUPPORT)
        wx_string_append(WXCONFIG_CPPFLAGS " -D_FILE_OFFSET_BITS=64")
    endif()
    string(STRIP "${WXCONFIG_CPPFLAGS}" WXCONFIG_CPPFLAGS)
    set(WXCONFIG_CXXFLAGS ${WXCONFIG_CFLAGS})
    set(WXCONFIG_LDFLAGS_GUI)
    set(WXCONFIG_RESFLAGS)
    set(WXCONFIG_RPATH "-Wl,-rpath,\$libdir")
    set(LDFLAGS_GL)
    set(RESCOMP)

    wx_configure_script(
        "wx-config.in"
        "${TOOLCHAIN_FULLNAME}"
        )
endfunction()

wx_write_config_inplace()
wx_write_config()

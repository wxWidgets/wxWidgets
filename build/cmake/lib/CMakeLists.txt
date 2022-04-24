#############################################################################
# Name:        build/cmake/lib/CMakeLists.txt
# Purpose:     Main lib CMake file
# Author:      Tobias Taschner
# Created:     2016-10-14
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(wxBUILD_MONOLITHIC)
    # Initialize variables for monolithic build
    set(wxMONO_SRC_FILES)
    set(wxMONO_LIBS_PRIVATE)
    set(wxMONO_LIBS_PUBLIC)
    set(wxMONO_INCLUDE_DIRS)
    set(wxMONO_DEFINITIONS)
    set(wxMONO_NONCOMPILED_CPP_FILES)
    set(wxMONO_DEPENDENCIES)
endif()

# Define third party libraries
set(LIBS_THIRDPARTY regex zlib expat)
if(wxUSE_GUI)
    list(APPEND LIBS_THIRDPARTY jpeg png tiff)
endif()
foreach(LIB IN LISTS LIBS_THIRDPARTY)
    include(${LIB}.cmake)
endforeach()

# add_opt_lib()
# Add library which may have been disabled by wxUSE_...
macro(add_opt_lib name var_name)
    if(${var_name})
        list(APPEND LIBS ${name})
    endif()
endmacro()

# Define base libraries
set(LIBS base)
add_opt_lib(net wxUSE_SOCKETS)

# Define UI libraries
if(wxUSE_GUI)
    list(APPEND LIBS core adv)
    foreach(lib
        aui
        html
        propgrid
        ribbon
        richtext
        webview
        stc
        xrc
        )
        string(TOUPPER ${lib} _name_upper)
        add_opt_lib(${lib} wxUSE_${_name_upper})
    endforeach()
    add_opt_lib(media wxUSE_MEDIACTRL)
    add_opt_lib(gl wxUSE_OPENGL)
    add_opt_lib(qa wxUSE_DEBUGREPORT)
endif() # wxUSE_GUI

# Include XML library last
# In the monolithic build, where all target properties (include dirs) from different targets are concatenated,
# wxml might include system expat, which might use Mono, which has it's own copy of png.
# Thus to ensure wx's core library includes the right png class, core must be processed first before xml
add_opt_lib(xml wxUSE_XML)

# Include cmake file for every library
foreach(LIB ${LIBS})
    add_subdirectory(${LIB})
endforeach()

if(wxBUILD_MONOLITHIC)
    # Create monolithic library target
    wx_add_library(wxmono IS_MONO ${wxMONO_SRC_FILES})
    foreach(vis PRIVATE PUBLIC)
        if(wxMONO_LIBS_${vis})
            # Remove libs included in mono from list
            foreach(lib IN LISTS LIBS)
                list(REMOVE_ITEM wxMONO_LIBS_${vis} wx${lib})
            endforeach()

            target_link_libraries(wxmono ${vis} ${wxMONO_LIBS_${vis}})
        endif()
    endforeach()
    if(wxMONO_INCLUDE_DIRS)
        target_include_directories(wxmono BEFORE PRIVATE ${wxMONO_INCLUDE_DIRS})
    endif()
    if(wxMONO_DEFINITIONS)
        target_compile_definitions(wxmono PRIVATE ${wxMONO_DEFINITIONS})
    endif()
    foreach(file ${wxMONO_NONCOMPILED_CPP_FILES})
        set_source_files_properties(${file} PROPERTIES HEADER_FILE_ONLY TRUE)
    endforeach()
    foreach(dep ${wxMONO_DEPENDENCIES})
        add_dependencies(wxmono ${dep})
    endforeach()
    if(wxUSE_WEBVIEW)
        wx_webview_copy_webview2_loader(wxmono)
    endif()
endif()

# Propagate variable(s) to parent scope
set(wxLIB_TARGETS ${wxLIB_TARGETS} PARENT_SCOPE)

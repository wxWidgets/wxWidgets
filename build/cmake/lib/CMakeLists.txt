#############################################################################
# Name:        build/cmake/lib/CMakeLists.txt
# Purpose:     Main lib CMake file
# Author:      Tobias Taschner
# Created:     2016-10-14
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../source_groups.cmake)

set(wxLIB_TARGETS)
if(wxBUILD_MONOLITHIC)
    # Initialize variables for monolithic build
    set(wxMONO_SRC_FILES)
    set(wxMONO_LIBS_PRIVATE)
    set(wxMONO_LIBS_PUBLIC)
    set(wxMONO_INCLUDE_DIRS_PRIVATE)
    set(wxMONO_INCLUDE_DIRS_PUBLIC)
endif()

# Define third party libraries
set(LIBS_THIRDPARTY regex zlib)
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
add_opt_lib(xml wxUSE_XML)

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

# Include cmake file for every library
foreach(LIB ${LIBS})
    add_subdirectory(${LIB})
endforeach()

if(wxBUILD_MONOLITHIC)
    # Create monolithic library target
    list(LENGTH wxMONO_SRC_FILES src_file_count)
    wx_add_library(mono ${wxMONO_SRC_FILES})
    foreach(vis PRIVATE PUBLIC)
        if(wxMONO_LIBS_${vis})
            # Remove libs included in mono from list
            foreach(lib IN LISTS LIBS)
                list(REMOVE_ITEM wxMONO_LIBS_${vis} ${lib})
            endforeach()

            target_link_libraries(mono ${vis} ${wxMONO_LIBS_${vis}})
        endif()
        if(wxMONO_INCLUDE_DIRS_${vis})
            target_include_directories(mono ${vis} ${wxMONO_INCLUDE_DIRS_${vis}})
        endif()
        if(wxMONO_DEFINITIONS_${vis})
            target_compile_definitions(mono ${vis} ${wxMONO_DEFINITIONS_${vis}})
        endif()
    endforeach()
endif()

# Propagate variable(s) to parent scope
set(wxLIB_TARGETS ${wxLIB_TARGETS} PARENT_SCOPE)
set(wxTHIRD_PARTY_LIBRARIES ${wxTHIRD_PARTY_LIBRARIES} PARENT_SCOPE)

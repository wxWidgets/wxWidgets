cmake_minimum_required(VERSION 3.21)

# This checks the public dispatch convention, not arbitrary C preprocessing.
# Compile-contract consumers additionally check which class actually wins when
# both toolkit macros are defined.
function(check_winui_dispatch content result)
    string(REGEX MATCH
        "#[ \t]*include[ \t]+[\"<]wx/winui/[^\">]+[\">]"
        winui_include "${content}")
    if(NOT winui_include)
        set(${result} "missing WinUI include" PARENT_SCOPE)
        return()
    endif()
    string(REGEX MATCH
        "#[ \t]*(if|elif)[ \t]+defined\\(__WXWINUI__\\)([ \t]+&&[ \t]+!defined\\(__WXUNIVERSAL__\\))?([ \t\r\n]|#[ \t]*define[^\n]*\n|//[^\n]*\n)+${winui_include}"
        guarded_include "${content}")
    if(NOT guarded_include)
        set(${result} "WinUI include lacks its dedicated toolkit branch"
            PARENT_SCOPE)
        return()
    endif()
    string(REGEX MATCH
        "#[ \t]*include[ \t]+[\"<]wx/msw/[^\">]+[\">]"
        msw_include "${content}")
    if(msw_include)
        string(FIND "${content}" "${winui_include}" winui_position)
        string(FIND "${content}" "${msw_include}" msw_position)
        if(msw_position LESS winui_position)
            set(${result} "MSW dispatch precedes WinUI dispatch" PARENT_SCOPE)
            return()
        endif()
    endif()
    set(${result} "" PARENT_SCOPE)
endfunction()

set(correct [=[
#if defined(__WXWINUI__)
    #include "wx/winui/button.h"
#elif defined(__WXMSW__)
    #include "wx/msw/button.h"
#endif
]=])
set(reversed [=[
#if defined(__WXMSW__)
    #include "wx/msw/button.h"
#elif defined(__WXWINUI__)
    #include "wx/winui/button.h"
#endif
]=])
check_winui_dispatch("${correct}" error)
if(error)
    message(FATAL_ERROR "Valid dispatch fixture rejected: ${error}")
endif()
check_winui_dispatch("${reversed}" error)
if(NOT error STREQUAL "MSW dispatch precedes WinUI dispatch")
    message(FATAL_ERROR "Reversed dispatch fixture was not rejected: ${error}")
endif()
string(REPLACE "defined(__WXWINUI__)" "defined(__WXMSW__)"
    wrong_guard "${correct}")
check_winui_dispatch("${wrong_guard}" error)
if(NOT error STREQUAL "WinUI include lacks its dedicated toolkit branch")
    message(FATAL_ERROR "Incorrect toolkit guard was not rejected: ${error}")
endif()

if(NOT IS_DIRECTORY "${SOURCE_DIR}/include/wx")
    message(FATAL_ERROR "Pass -DSOURCE_DIR=<wx source tree>")
endif()
file(GLOB headers "${SOURCE_DIR}/include/wx/*.h")
set(checked)
foreach(header IN LISTS headers)
    file(READ "${header}" content)
    if(content MATCHES "#[ \t]*include[ \t]+[\"<]wx/winui/")
        check_winui_dispatch("${content}" error)
        if(error)
            message(FATAL_ERROR "${header}: ${error}")
        endif()
        get_filename_component(name "${header}" NAME)
        list(APPEND checked "${name}")
    endif()
endforeach()
foreach(required IN ITEMS button.h radiobox.h spinbutt.h spinctrl.h)
    if(NOT required IN_LIST checked)
        message(FATAL_ERROR "Required dispatch header not checked: ${required}")
    endif()
endforeach()
list(LENGTH checked count)
message(STATUS "WinUI dispatch: ${count} public headers and negative fixtures pass")

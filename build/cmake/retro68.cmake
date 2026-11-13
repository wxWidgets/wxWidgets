#############################################################################
# Name:        build/cmake/retro68.cmake
# Purpose:     Default settings for building with the Retro68 toolchain
# Created:     2026-11-12
# Copyright:   (c) 2026 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# Retro68 (https://github.com/autc04/Retro68) is a GCC-based cross compiler
# targeting classic Mac OS (System 6/7 up to Mac OS 9) on 68K (CMAKE_SYSTEM_NAME
# "Retro68") and PowerPC ("RetroPPC" for the classic Toolbox and
# "RetroCarbon" for CarbonLib) machines.
#
# Use it by passing one of the toolchain files shipped with Retro68, e.g.
#
#   cmake -DCMAKE_TOOLCHAIN_FILE=/opt/retro68/m68k-apple-macos/cmake/retro68.toolchain.cmake ..
#   cmake -DCMAKE_TOOLCHAIN_FILE=/opt/retro68/powerpc-apple-macos/cmake/retroppc.toolchain.cmake ..
#
# The values below are only defaults and can still be overridden on the
# command line, but most of them correspond to features that simply don't
# exist under classic Mac OS.

set(RETRO68 ON)
if(CMAKE_SYSTEM_NAME STREQUAL "Retro68")
    set(RETRO68_M68K ON)
else()
    set(RETRO68_PPC ON)
    if(CMAKE_SYSTEM_NAME STREQUAL "RetroCarbon")
        set(RETRO68_CARBON ON)
    endif()
endif()

# CMake can't parse the Mac executables produced by the toolchain to determine
# the pointer size and byte order, but they're always the same for both 68K
# and PowerPC.
if(NOT CMAKE_SIZEOF_VOID_P)
    set(CMAKE_SIZEOF_VOID_P 4)
endif()
foreach(lang C CXX)
    if(NOT CMAKE_${lang}_BYTE_ORDER)
        set(CMAKE_${lang}_BYTE_ORDER BIG_ENDIAN)
    endif()
endforeach()

# For the same reason check_type_size() doesn't work neither, so predefine the
# results of all the checks done by our CMake code.
if(RETRO68_M68K)
    set(wxRETRO68_WCHAR_T_SIZE 4)
else()
    set(wxRETRO68_WCHAR_T_SIZE 2)
endif()
foreach(type_size
        SIZEOF_SIZE_T=4 SIZEOF_WCHAR_T=${wxRETRO68_WCHAR_T_SIZE} SIZEOF_INT=4
        SIZEOF_LONG=4 SIZEOF_SHORT=2 SIZEOF_LONG_LONG=8 SSIZE_T=4)
    string(REPLACE "=" ";" type_size ${type_size})
    list(GET type_size 0 var)
    list(GET type_size 1 size)
    set(HAVE_${var} TRUE CACHE INTERNAL "")
    set(${var} ${size} CACHE INTERNAL "")
endforeach()

# Only static libraries are supported.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
set(wxBUILD_SHARED OFF CACHE BOOL "Build wx libraries as shared libs")
# Position independent code is not needed for static libraries.
set(wxBUILD_PIC OFF CACHE BOOL "Enable position independent code (PIC).")
# Precompiled headers don't bring much and are fragile when cross-compiling.
set(wxBUILD_PRECOMP OFF CACHE STRING "Use precompiled headers")
# Make sure that all static libraries go into the same, simple location.
set(wxBUILD_INSTALL_PLATFORM_SUBDIR OFF CACHE BOOL "use platform specific sub-directory")

macro(wx_retro68_default name value)
    if(NOT DEFINED ${name})
        set(${name} ${value} CACHE STRING "Default for Retro68")
    endif()
endmacro()

# Use built-in versions of all third party libraries, there are no system
# ones (except for zlib, but use the built-in one for consistency).
wx_retro68_default(wxUSE_SYS_LIBS OFF)
wx_retro68_default(wxUSE_LIBLZMA OFF)
wx_retro68_default(wxUSE_LIBTIFF OFF)
wx_retro68_default(wxUSE_LIBWEBP OFF)
wx_retro68_default(wxUSE_LUNASVG OFF)
wx_retro68_default(wxUSE_OPENGL OFF)

# Operating system features not available under classic Mac OS.
wx_retro68_default(wxUSE_THREADS OFF)
wx_retro68_default(wxUSE_SOCKETS OFF)
wx_retro68_default(wxUSE_IPV6 OFF)
wx_retro68_default(wxUSE_IPC OFF)
wx_retro68_default(wxUSE_URL OFF)
wx_retro68_default(wxUSE_PROTOCOL OFF)
wx_retro68_default(wxUSE_FS_INET OFF)
wx_retro68_default(wxUSE_WEBREQUEST OFF)
wx_retro68_default(wxUSE_DIALUP_MANAGER OFF)
wx_retro68_default(wxUSE_DYNLIB_CLASS OFF)
wx_retro68_default(wxUSE_DYNAMIC_LOADER OFF)
wx_retro68_default(wxUSE_FSWATCHER OFF)
wx_retro68_default(wxUSE_FSVOLUME OFF)
wx_retro68_default(wxUSE_MIMETYPE OFF)
wx_retro68_default(wxUSE_SNGLINST_CHECKER OFF)
wx_retro68_default(wxUSE_STACKWALKER OFF)
wx_retro68_default(wxUSE_ON_FATAL_EXCEPTION OFF)
wx_retro68_default(wxUSE_DEBUGREPORT OFF)
wx_retro68_default(wxUSE_CONSOLE_EVENTLOOP OFF)
wx_retro68_default(wxUSE_XLOCALE OFF)
wx_retro68_default(wxUSE_SOUND OFF)
wx_retro68_default(wxUSE_SPELLCHECK OFF)
wx_retro68_default(wxUSE_VISIBILITY OFF)

# GUI features which either make no sense or aren't implemented (yet).
wx_retro68_default(wxUSE_MEDIACTRL OFF)
wx_retro68_default(wxUSE_WEBVIEW OFF)
wx_retro68_default(wxUSE_GRAPHICS_CONTEXT OFF)
wx_retro68_default(wxUSE_CAIRO OFF)
wx_retro68_default(wxUSE_PRINTING_ARCHITECTURE OFF)
wx_retro68_default(wxUSE_POSTSCRIPT OFF)
wx_retro68_default(wxUSE_DRAG_AND_DROP OFF)
# Clipboard and data objects are not implemented yet.
wx_retro68_default(wxUSE_CLIPBOARD OFF)
wx_retro68_default(wxUSE_TOOLTIPS OFF)
wx_retro68_default(wxUSE_FILECTRL OFF)
wx_retro68_default(wxUSE_DATAOBJ OFF)
wx_retro68_default(wxUSE_ACCESSIBILITY OFF)
wx_retro68_default(wxUSE_JOYSTICK OFF)
wx_retro68_default(wxUSE_HOTKEY OFF)
wx_retro68_default(wxUSE_METAFILE OFF)
wx_retro68_default(wxUSE_UIACTIONSIMULATOR OFF)
wx_retro68_default(wxUSE_TASKBARICON OFF)
wx_retro68_default(wxUSE_TASKBARBUTTON OFF)
wx_retro68_default(wxUSE_NOTIFICATION_MESSAGE OFF)
wx_retro68_default(wxUSE_PRIVATE_FONTS OFF)
wx_retro68_default(wxUSE_ACTIVITYINDICATOR OFF)
wx_retro68_default(wxUSE_STC OFF)
wx_retro68_default(wxUSE_LIBMSPACK OFF)

# Resource file overriding the default SIZE resource of Retro68 applications.
set(wxRETRO68_REZ_FILE "${CMAKE_CURRENT_LIST_DIR}/../../src/retro68/wxretro68.r")
get_filename_component(wxRETRO68_REZ_FILE "${wxRETRO68_REZ_FILE}" ABSOLUTE)

# Allow the linker to discard unused functions and data, which significantly
# reduces the size of the applications using wxWidgets.
add_compile_options(-ffunction-sections -fdata-sections)

# Package the given executable target as a classic Mac OS application, i.e.
# do the same thing as add_application() from Retro68 does, but for an
# existing target. This creates <target>.bin (MacBinary), <target>.dsk (HFS
# disk image containing the application) and <target>.APPL files next to the
# executable.
function(wx_retro68_package_application target)
    cmake_parse_arguments(APP "CONSOLE" "CREATOR" "" ${ARGN})

    if(NOT APP_CREATOR)
        set(APP_CREATOR "????")
    endif()

    target_link_options(${target} PRIVATE "-Wl,--gc-sections")

    if(APP_CONSOLE)
        if(RETRO68_CARBON)
            target_link_libraries(${target} PRIVATE RetroConsoleCarbon)
        else()
            target_link_libraries(${target} PRIVATE RetroConsole)
        endif()
    endif()

    set(rez_includes -I${REZ_INCLUDE_PATH})
    set(out_dir "$<TARGET_FILE_DIR:${target}>")

    if(RETRO68_M68K)
        set_target_properties(${target} PROPERTIES OUTPUT_NAME "${target}.code.bin")

        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${REZ}
                    ${REZ_TEMPLATES_PATH}/Retro68APPL.r
                    ${wxRETRO68_REZ_FILE}
                    ${rez_includes}
                    --copy "$<TARGET_FILE:${target}>"
                    -o "${out_dir}/${target}.bin"
                    -t APPL -c ${APP_CREATOR}
                    --cc "${out_dir}/${target}.dsk"
                    --cc "${out_dir}/${target}.APPL"
            COMMENT "Creating ${target} application"
            VERBATIM)
    else()
        set_target_properties(${target} PROPERTIES OUTPUT_NAME "${target}.xcoff")

        set(rez_flags)
        set(rez_template "${REZ_TEMPLATES_PATH}/RetroPPCAPPL.r")
        if(RETRO68_CARBON)
            set(rez_flags -DTARGET_API_MAC_CARBON=1)
            set(rez_template "${REZ_TEMPLATES_PATH}/RetroCarbonAPPL.r")
        endif()

        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${MAKE_PEF} "$<TARGET_FILE:${target}>"
                    -o "${out_dir}/${target}.pef"
            COMMAND ${REZ}
                    ${rez_flags}
                    ${rez_template}
                    ${wxRETRO68_REZ_FILE}
                    ${rez_includes}
                    "-DCFRAG_NAME=\"${target}\""
                    -o "${out_dir}/${target}.bin"
                    -t APPL -c ${APP_CREATOR}
                    --cc "${out_dir}/${target}.dsk"
                    --cc "${out_dir}/${target}.APPL"
                    --data "${out_dir}/${target}.pef"
            COMMENT "Creating ${target} application"
            VERBATIM)
    endif()
endfunction()

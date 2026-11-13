#############################################################################
# Name:        build/cmake/uninstall.cmake.in
# Purpose:     CMake uinstall template
# Author:      Tobias Taschner
# Created:     2016-10-18
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(NOT EXISTS "/Users/andyvand/Downloads/wxWidgets/build-m68k/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: /Users/andyvand/Downloads/wxWidgets/build-m68k/install_manifest.txt")
endif()

file(READ "/Users/andyvand/Downloads/wxWidgets/build-m68k/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
list(APPEND files "/opt/retro68/m68k-apple-macos/bin/wxrc";"/opt/retro68/m68k-apple-macos/bin/wx-config")
foreach(file ${files})
  message(STATUS "Uninstalling $ENV{DESTDIR}${file}")
  if(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    exec_program(
      "/Applications/CMake.app/Contents/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    if(NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing $ENV{DESTDIR}${file}")
    endif(NOT "${rm_retval}" STREQUAL 0)
  else()
    message(STATUS "File $ENV{DESTDIR}${file} does not exist.")
  endif()
endforeach(file)

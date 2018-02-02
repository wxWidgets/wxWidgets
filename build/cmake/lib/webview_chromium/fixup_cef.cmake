#############################################################################
# Name:        build/cmake/lib/webview/fixup_cef.cmake
# Purpose:     CMake script to up patch CEF
# Author:      Tobias Taschner
# Created:     2018-02-03
# Copyright:   (c) 2018 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# This script modifies CEF cmake files to allow different VC runtime settings

set(var_file cmake/cef_variables.cmake)

file(READ ${var_file} var_file_contents)
string(REGEX REPLACE "/MTd?" "" new_file_contents ${var_file_contents})
file(WRITE "${var_file}" ${new_file_contents})

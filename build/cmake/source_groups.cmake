#############################################################################
# Name:        build/cmake/source_groups.cmake
# Purpose:     CMake source groups file
# Author:      Tobias Taschner
# Created:     2016-10-14
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# Define source groups for supported IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(wx_set_source_groups)
    source_group("Common Headers" REGULAR_EXPRESSION "/include/wx/.*\\.h")
    source_group("Common Sources" REGULAR_EXPRESSION "/src/common/.*")
    source_group("GTK+ Sources" REGULAR_EXPRESSION "/src/gtk/.*")
    source_group("MSW Sources" REGULAR_EXPRESSION "/src/msw/.*")
    source_group("OSX Sources" REGULAR_EXPRESSION "/src/osx/.*")
    source_group("Generic Sources" REGULAR_EXPRESSION "/src/generic/.*")
    source_group("wxUniv Sources" REGULAR_EXPRESSION "/src/univ/.*")
    source_group("wxHTML Sources" REGULAR_EXPRESSION "/src/html/.*")
    source_group("Setup Headers" REGULAR_EXPRESSION "/include/.*/setup.h")
    source_group("GTK+ Headers" REGULAR_EXPRESSION "/include/wx/gtk/.*")
    source_group("MSW Headers" REGULAR_EXPRESSION "/include/wx/msw/.*")
    source_group("OSX Headers" REGULAR_EXPRESSION "/include/wx/osx/.*")
    source_group("Generic Headers" REGULAR_EXPRESSION "/include/wx/generic/.*")
    source_group("wxUniv Headers" REGULAR_EXPRESSION "/include/wx/univ/.*")
    source_group("wxHTML Headers" REGULAR_EXPRESSION "/include/wx/html/.*")
    source_group("Setup Headers" FILES ${wxSETUP_HEADER_FILE})
    source_group("Resource Files" REGULAR_EXPRESSION "/[^.]*.(rc|ico|png|icns|manifest|plist)$")
    source_group("CMake" REGULAR_EXPRESSION "(CMakeLists\\.txt|cmake_pch.*|.*_CXX_prefix\\.hxx\\.rule)$")
endfunction()

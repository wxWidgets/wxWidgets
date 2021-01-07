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
source_group("Common Headers" REGULAR_EXPRESSION "${wxSOURCE_DIR}/include/wx/.*\\.h")
source_group("Common Sources" REGULAR_EXPRESSION "${wxSOURCE_DIR}/src/common/.*")
source_group("GTK+ Sources" REGULAR_EXPRESSION "${wxSOURCE_DIR}/src/gtk/.*")
source_group("MSW Sources" REGULAR_EXPRESSION "${wxSOURCE_DIR}/src/msw/.*")
source_group("OSX Sources" REGULAR_EXPRESSION "${wxSOURCE_DIR}/src/osx/.*")
source_group("Generic Sources" REGULAR_EXPRESSION "${wxSOURCE_DIR}/src/generic/.*")
source_group("wxUniv Sources" REGULAR_EXPRESSION "${wxSOURCE_DIR}/src/univ/.*")
source_group("wxHTML Sources" REGULAR_EXPRESSION "${wxSOURCE_DIR}/src/html/.*")
source_group("Setup Headers" REGULAR_EXPRESSION "${wxSOURCE_DIR}/include/.*/setup.h")
source_group("GTK+ Headers" REGULAR_EXPRESSION "${wxSOURCE_DIR}/include/wx/gtk/.*")
source_group("MSW Headers" REGULAR_EXPRESSION "${wxSOURCE_DIR}/include/wx/msw/.*")
source_group("OSX Headers" REGULAR_EXPRESSION "${wxSOURCE_DIR}/include/wx/osx/.*")
source_group("Generic Headers" REGULAR_EXPRESSION "${wxSOURCE_DIR}/include/wx/generic/.*")
source_group("wxUniv Headers" REGULAR_EXPRESSION "${wxSOURCE_DIR}/include/wx/univ/.*")
source_group("wxHTML Headers" REGULAR_EXPRESSION "${wxSOURCE_DIR}/include/wx/html/.*")
source_group("Setup Headers" FILES ${wxSETUP_HEADER_FILE})
source_group("Resource Files" REGULAR_EXPRESSION "${wxSOURCE_DIR}/[^.]*.(rc|ico|png|icns)$")

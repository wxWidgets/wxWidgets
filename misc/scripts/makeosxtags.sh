#!/bin/sh
. `dirname $0`/makeunixtags.sh

osx_port=${1-carbon}
ctags --totals --c++-kinds=+px --language-force=c++ \
    -f osx_$osx_port.tags \
    -I @misc/scripts/ctags.ignore \
    include/wx/osx/*.h \
    include/wx/osx/core/*.h \
    include/wx/osx/core/private/*.h \
    include/wx/osx/private/*.h \
    include/wx/osx/$osx_port/*.h \
    include/wx/osx/$osx_port/private/*.h \
    src/osx/*.cpp \
    src/osx/core/*.cpp \
    src/osx/$osx_port/*.{cpp,mm}

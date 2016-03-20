#!/bin/sh
. `dirname $0`/makeunixtags.sh
create_tags osx

ctags --totals --c++-kinds=+px --language-force=c++ \
    -a \
    -I @misc/scripts/ctags.ignore \
    include/wx/osx/core/*.h \
    include/wx/osx/core/private/*.h \
    include/wx/osx/private/*.h \
    include/wx/osx/cocoa/*.h \
    include/wx/osx/cocoa/private/*.h \
    src/osx/core/*.cpp \
    src/osx/cocoa/*.mm

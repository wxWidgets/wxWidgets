#!/bin/bash  

SRCLIST='source.list'
find src/gtk src/gtk3 src/common src/generic src/unix include/wx/gtk include/wx/generic include/wx/unix -name "*.cpp" -o -name "*.h" > $SRCLIST
find -L cinclude -name "*.h" >> $SRCLIST
find include/wx -maxdepth 1 -name "*.h" >> $SRCLIST
gtags -f $SRCLIST
ctags -R --c++-kinds=+p --fields=+ialS --extra=+q --language-force=c++ -L $SRCLIST

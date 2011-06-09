#!/bin/bash  

SRCLIST='source.list'
find src/gtk src/common src/generic src/unix include/wx/gtk include/wx/generic include/wx/unix -name "*.cpp" -o -name "*.h" > $SRCLIST
find -L cinclude -name "*.h" >> $SRCLIST
gtags -f $SRCLIST
ctags -R --c++-kinds=+p --fields=+ialS --extra=+q --language-force=c++ -L $SRCLIST

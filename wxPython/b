#!/bin/sh

if [ "$1" = "15" ]; then
    PYVER=1.5
    shift
elif [ "$1" = "20" ]; then
    PYVER=2.0
    shift
fi


SETUP="python$PYVER -u setup.py"
FLAGS="USE_SWIG=1 IN_CVS_TREE=1"
OTHERFLAGS=""



# "c" --> clean
if [ "$1" =  "c" ]; then
    shift
    CMD="$SETUP $FLAGS $OTHERFLAGS clean"
    OTHERCMD="rm -f wxPython/*.so"

# "i" --> install
elif [ "$1" = "i" ]; then
    shift
    CMD="$SETUP build $OTHERFLAGS install"

# "s" --> source dist
elif [ "$1" = "s" ]; then
    shift
    CMD="$SETUP $OTHERFLAGS sdist"

# "r" --> rpm dist
elif [ "$1" = "r" ]; then
    shift
    CMD="$SETUP $OTHERFLAGS bdist_rpm"

# (no command arg) --> normal build for development
else
    CMD="$SETUP $FLAGS $OTHERFLAGS build_ext --inplace $*"
fi


echo $CMD
$CMD


if [ "$OTHERCMD" != "" ]; then
    echo $OTHERCMD
    $OTHERCMD
fi


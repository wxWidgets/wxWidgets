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

    # save the original
    cp setup.py setup.py.save

    # fix up setup.py the way we want...
    sed "s/BUILD_GLCANVAS = /BUILD_GLCANVAS = 0 #/" < setup.py.save > setup.py.temp
    sed "s/GL_ONLY = /GL_ONLY = 1 #/" < setup.py.temp > setup.py

    # build wxPython-gl RPM
    $SETUP $OTHERFLAGS bdist_rpm --binary-only --doc-files README.txt
    rm dist/wxPython-gl*.tar.gz

    # Build wxPython RPM
    cp setup.py setup.py.temp
    sed "s/GL_ONLY = /GL_ONLY = 0 #/" < setup.py.temp > setup.py
    $SETUP $OTHERFLAGS bdist_rpm

    # put the oringal back
    cp setup.py.save setup.py
    rm setup.py.*

    # rebuild the source dist without the munched up setup.py
    $SETUP $OTHERFLAGS sdist
    exit 0


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


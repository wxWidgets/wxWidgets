#!/bin/bash

# Are we using bash on win32?  If so source that file and then exit.
if [ "$OSTYPE" = "cygwin" ]; then
    source b.win32
    exit
fi

# make it easy to switch versions of SWIG
if [ "$SWIG" = "" ]; then
  SWIG=/opt/swig/bin/swig-1.3.27
fi


function getpyver {
    if [ "$1" = "15" ]; then
	PYVER=1.5
    elif [ "$1" = "20" ]; then
	PYVER=2.0
    elif [ "$1" = "21" ]; then
	PYVER=2.1
    elif [ "$1" = "22" ]; then
	PYVER=2.2
    elif [ "$1" = "23" ]; then
	PYVER=2.3
    elif [ "$1" = "24" ]; then
	PYVER=2.4
    elif [ "$1" = "25" ]; then
	PYVER=2.5
    else
	echo You must specify Python version as first parameter.
        exit
    fi
}

getpyver $1
shift

python$PYVER -c "import sys;print '\n', sys.version, '\n'"


SETUP="python$PYVER -u setup.py"
FLAGS="USE_SWIG=1 SWIG=$SWIG" 
OTHERFLAGS=""
PORTFLAGS=""
UNIFLAG="UNICODE=1"



if [ "$1" = "gtk1" -o "$1" = "gtk" ]; then 
    PORTFLAGS="WXPORT=gtk"
    UNIFLAG="UNICODE=0"
    shift
elif [ "$1" = "gtk2" ]; then 
    PORTFLAGS="WXPORT=gtk2"
    UNIFLAG="UNICODE=1"
    shift
fi

for p in $*; do
    if [ "$p" = "UNICODE=0" -o "$p" = "UNICODE=1" ]; then
	UNIFLAG=""
	break
    fi
done

FLAGS="$FLAGS $PORTFLAGS $UNIFLAG"




# "c" --> clean
if [ "$1" =  "c" ]; then
    shift
    CMD="$SETUP $FLAGS $OTHERFLAGS clean $*"
    OTHERCMD="rm -f wx/*.so"

# "d" --> clean extension modules only
elif [ "$1" = "d" ]; then
    shift
    CMD="rm -f wx/*.so"

# "t" --> touch *.i files
elif [ "$1" = "t" ]; then
    shift
    CMD='find . -name "*.i" | xargs touch'

# "i" --> install
elif [ "$1" = "i" ]; then
    shift
    CMD="$SETUP $FLAGS $OTHERFLAGS build_ext install $*"

# "s" --> source dist
elif [ "$1" = "s" ]; then
    shift
    CMD="$SETUP $OTHERFLAGS sdist $*"

# "r" --> rpm dist
elif [ "$1" = "r" ]; then
    WXPYVER=`python$PYVER -c "import setup;print setup.VERSION"`
    for VER in 21 22; do
	getpyver $VER

	echo "*****************************************************************"
	echo "*******      Building wxPython for Python $PYVER"
	echo "*****************************************************************"

	SETUP="python$PYVER -u setup.py"

	# save the original
	cp setup.py setup.py.save

	# fix up setup.py the way we want...
	sed "s/BUILD_GLCANVAS = /BUILD_GLCANVAS = 0 #/" < setup.py.save > setup.py.temp
	sed "s/GL_ONLY = /GL_ONLY = 1 #/" < setup.py.temp > setup.py

	# build wxPython-gl RPM
	$SETUP $OTHERFLAGS bdist_rpm --binary-only --doc-files README.txt --python=python$PYVER
			### --requires=python$PYVER
	rm dist/wxPython-gl*.tar.gz

	# Build wxPython RPM
	cp setup.py setup.py.temp
	sed "s/GL_ONLY = /GL_ONLY = 0 #/" < setup.py.temp > setup.py
	$SETUP $OTHERFLAGS bdist_rpm --binary-only --python=python$PYVER
			### --requires=python$PYVER

	# put the oringal setup.py back
	cp setup.py.save setup.py
	rm setup.py.*

	# rename the binary RPM's
	mv dist/wxPython-$WXPYVER-1.i386.rpm dist/wxPython-$WXPYVER-1-Py$VER.i386.rpm
	mv dist/wxPython-gl-$WXPYVER-1.i386.rpm dist/wxPython-gl-$WXPYVER-1-Py$VER.i386.rpm

    done

    # rebuild the source dists without the munched up setup.py
    $SETUP $OTHERFLAGS bdist_rpm --source-only
    exit 0


# "f" --> FINAL (no debug)
elif [ "$1" = "f" ]; then
    shift
    CMD="$SETUP $FLAGS $OTHERFLAGS build_ext --inplace $*"

# (no command arg) --> normal build for development
else
    CMD="$SETUP $FLAGS $OTHERFLAGS build_ext --inplace --debug $*"
fi


echo $CMD
eval $CMD
RC=$?

if [ "$RC" = "0" -a  "$OTHERCMD" != "" ]; then
    echo $OTHERCMD
    $OTHERCMD
    RC=$?
fi

exit $RC

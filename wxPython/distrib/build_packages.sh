#!/bin/bash

unicode=no
debug=no
reswig=no
all=no

if [ "$1" = "all" ]; then 
  all=yes
else
  PY_VERSION=$1
fi

shift 

for flag in $*; do
    case ${flag} in
      debug)       debug=yes              ;;
      unicode)     unicode=yes            ;;
      reswig)      reswig=yes             ;;
    esac
done

if [ "$WXWIN" = "" ]; then
  export WXWIN=`pwd`/../..
fi

if [ $all = yes ]; then
  $WXWIN/wxPython/distrib/build_packages.sh 23
  $WXWIN/wxPython/distrib/build_packages.sh 23 unicode
  $WXWIN/wxPython/distrib/build_packages.sh 24
  $WXWIN/wxPython/distrib/build_packages.sh 24 unicode
fi

echo "wxWidgets directory is: $WXWIN"

if [ "$OSTYPE" = "cygwin" ]; then
  # do setup of build environment vars
  if [ "$TOOLS" = "" ]; then
    export TOOLS=`cygpath C:\\`
  fi

  if [ "$SWIGDIR" = "" ]; then
    export SWIGDIR=$TOOLS/SWIG-1.3.24
  fi

  DEBUG_FLAG=
  UNICODE_FLAG=
  if [ $debug = yes ]; then
    DEBUG_FLAG=--debug
  fi
  if [ $unicode = yes ]; then
    UNICODE_FLAG="UNICODE=1"
  fi

  # copy wxPython build scripts
  cp $WXWIN/wxPython/distrib/msw/.m* $WXWIN/build/msw
  
  # setup wxPython defines
  cp $WXWIN/include/wx/msw/setup0.h $WXWIN/include/wx/msw/setup.h
  $TOOLS/Python$PY_VERSION/python `cygpath -d $WXWIN/wxPython/distrib/create_setup.h.py` $UNICODE_FLAG
  
  export PATH=${PATH}:${WXWIN}/lib/vc_dll:${TOOLS}/Python${PY_VERSION}
  
  cd $WXWIN/build/msw
  # remove old build files
  rm -rf vc_msw*
  UNI=
  if [ $unicode = yes ]; then
      UNI=-uni
  fi
  ./.make hybrid$UNI
  # make tools for docs creation, etc.
  ./.make_tools
  
  cd $WXWIN/wxPython

  # update the language files
  $TOOLS/Python$PY_VERSION/python `cygpath -d $WXWIN/wxPython/distrib/makemo.py`
  rm -rf build build.unicode
  rm -rf wx/*.pyd
  
  # re-generate SWIG files
  if [ $reswig = yes ]; then
    $WXWIN/wxPython/b $PY_VERSION t
  fi
  
  # build the hybrid extension
  # NOTE: Win Python needs Windows-style pathnames, so we 
  # need to convert
  export WXWIN=`cygpath -w $WXWIN`
  export SWIGDIR=`cygpath -w $SWIGDIR`
  
  $WXWIN/wxPython/b $PY_VERSION h $DEBUG_FLAG $UNICODE_FLAG
  
  # make the dev package
  $WXWIN/wxPython/distrib/makedev
  $WXWIN/wxPython/distrib/makedocs
  $WXWIN/wxPython/distrib/makedemo
  
  $TOOLS/Python$PY_VERSION/python `cygpath -d $WXWIN/wxPython/distrib/make_installer_inno4.py` $UNICODE_FLAG
elif [ "$OSTYPE" = "darwin" ]; then
  OSX_VERSION=`sw_vers -productVersion`
  echo "OS X Version: ${OSX_VERSION:0:4}"
  cd $WXWIN/wxPython
  
  if [ ! -d dist ]; then
    mkdir dist
  fi
  # re-generate SWIG files
  RESWIG=
  if [ $reswig = yes ]; then
    RESWIG=reswig
  fi
  
  PY_DOT_VER=2.3
  if [ "$PY_VERSION" = "24" ]; then
    PY_DOT_VER=2.4
  fi
  
  UNICODE_OPT=
  if [ $unicode = yes ]; then
    UNICODE_OPT=unicode
  fi 
  
  DEBUG_OPT=
  if [ $debug = yes ]; then
    DEBUG_OPT=debug
  fi
  # On Tiger, build Universal.
  UNIV_OPT=
  if [ ${OSX_VERSION:0:4} = "10.4" ]; then
    UNIV_OPT="universal"
  fi
  
  #sudo $WXWIN/wxPython/distrib/makedocs
  $WXWIN/wxPython/distrib/makedemo
  export TARBALLDIR=$WXWIN/wxPython/dist
  echo "distrib/mac/wxPythonOSX/build $PY_DOT_VER inplace $UNICODE_OPT $RESWIG"
  
  distrib/mac/wxPythonOSX/build $PY_DOT_VER inplace $UNICODE_OPT $DEBUG_OPT $RESWIG $UNIV_OPT
else
  echo "OSTYPE $OSTYPE not yet supported by this build script."
fi

# return to original dir
cd $WXWIN/wxPython/distrib


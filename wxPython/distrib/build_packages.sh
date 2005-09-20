#!/bin/bash

PY_VERSION=$1
shift

if [ "$WXWIN" = "" ]; then
  export WXWIN=`pwd`/../..
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

  # copy wxPython build scripts
  cp $WXWIN/wxPython/distrib/msw/.m* $WXWIN/build/msw
  
  # setup wxPython defines
  cp $WXWIN/include/wx/msw/setup0.h $WXWIN/include/wx/msw/setup.h
  $TOOLS/Python$PY_VERSION/python `cygpath -d $WXWIN/wxPython/distrib/create_setup.h.py` $@
  
  export PATH=${PATH}:${WXWIN}/lib/vc_dll
  
  cd $WXWIN/build/msw
  # remove old build files
  rm -rf vc_msw*
  UNI=
  if [ "$UNICODE" != "" ]; then
      UNI=-uni
  fi
  ./.make hybrid$UNI
  
  # make tools for docs creation, etc.
  ./.make_tools
  
  # update the language files
  cd $WXWIN/locale
  make allmo
  
  # TODO: Make the documentation
  cd $WXWIN/wxPython
  #distrib/makedocs

  $TOOLS/Python$PY_VERSION/python `cygpath -d distrib/makemo.py`

  rm -rf build build.unicode
  rm -rf wx/*.pyd
  
  # re-generate SWIG files
  b $PY_VERSION t
  
  # build the hybrid extension
  # NOTE: Win Python needs Windows-style pathnames, so we 
  # need to convert
  export WXWIN=`cygpath -d $WXWIN`
  export SWIGDIR=`cygpath -d $SWIGDIR`
  
  DEBUG_FLAG=
  UNICODE_FLAG=
  if [ "$DEBUG" != "" ]; then
    DEBUG_FLAG=--debug
  fi
  if [ "$UNICODE" != "" ]; then
    UNICODE_FLAG="UNICODE=1"
  fi
  b $PY_VERSION h $DEBUG_FLAG $UNICODE_FLAG
  
  # make the dev package
  #distrib/makedev
  
  $TOOLS/Python$PY_VERSION/python distrib/make_installer_inno4.py $UNICODE_FLAG
  exit
elif [ "$OSTYPE" = "darwin" ]; then
  cd $WXWIN/wxPython
  
  # re-generate SWIG files
  ./b $PY_VERSION t
  
  sudo distrib/mac/wxPythonOSX/build 2.3 panther inplace
  exit
else
  echo "OSTYPE $OSTYPE not yet supported by this build script."
fi

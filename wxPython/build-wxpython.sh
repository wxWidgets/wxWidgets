#!/bin/bash

unicode=no
debug=no
reswig=no
all=no

if [ "$1" = all ]; then
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

scriptDir="$(cd $(dirname $0);pwd)"
scriptName="$(basename $0)"

if [ "$WXWIN" = "" ]; then
  export WXWIN=$scriptDir/..
fi

if [ $all = yes ]; then
  ${scriptDir}/${scriptName}.sh 23
  ${scriptDir}/${scriptName}.sh 23 unicode
  ${scriptDir}/${scriptName}.sh 24
  ${scriptDir}/${scriptName}.sh 24 unicode
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
else
  UNICODE_OPT=
  UNICODE_WXPY_OPT=0
  if [ $unicode = yes ]; then
    UNICODE_OPT=unicode
    UNICODE_WXPY_OPT=1
  fi 
  
  DEBUG_OPT=
  if [ $debug = yes ]; then
    DEBUG_OPT=debug
  fi

  mkdir -p wxpy-bld
  cd wxpy-bld
  BUILD_DIR=$PWD
  export INSTALLDIR=$HOME/wxpython-2.8.4
  if [ "${OSTYPE:0:6}" = "darwin" ]; then
    $WXWIN/distrib/scripts/mac/macbuild-lipo wxpython $UNICODE_OPT $DEBUG_OPT
  else
    $WXWIN/distrib/scripts/unix/unixbuild wxpython $UNICODE_OPT $DEBUG_OPT
  fi
  
  if [ $? != 0 ]; then
    exit $?
  fi

  cd $scriptDir
  python$PY_VERSION ./setup.py build_ext --inplace WX_CONFIG=$BUILD_DIR/wx-config UNICODE=$UNICODE_WXPY_OPT
fi

# return to original dir
cd $WXWIN/wxPython


#!/bin/bash

# set -o xtrace

function error {
    echo -----------------
    echo --    ERROR!   --
    echo -----------------
    exit $1
}


if [ "$1" == "both" ]; then
    shift
    .make debug $@   &&   .make hybrid $@
    exit $?

elif [ "$1" == "both-uni" ]; then
    shift
    .make debug-uni $@  &&  .make hybrid-uni $@ 
    exit $?

elif [ "$1" == "both-hybrid" ]; then
    shift
    .make hybrid $@    &&  .make hybrid-uni $@
    exit $?

elif [ "$1" == "all" ]; then
    shift
    .make debug $@          && \
	.make hybrid $@     && \
	.make debug-uni $@  && \
	.make hybrid-uni $@ 
    exit $?

elif [ "$1" == "debug" ]; then
    BUILD_TYPE=$1
    WXLIBDIR=mswd
    shift

elif [ "$1" == "hybrid" ]; then
    BUILD_TYPE=$1
    WXLIBDIR=mswh
    shift

elif [ "$1" == "release" ]; then
    BUILD_TYPE=$1
    WXLIBDIR=msw
    shift

elif [ "$1" == "debug-uni" ]; then
    BUILD_TYPE=$1
    WXLIBDIR=mswud
    shift

elif [ "$1" == "hybrid-uni" ]; then
    BUILD_TYPE=$1
    WXLIBDIR=mswuh
    shift

elif [ "$1" == "release-uni" ]; then
    BUILD_TYPE=$1
    WXLIBDIR=mswu
    shift

else
    echo "Invalid build type: $1  options are"
    echo "    debug"
    echo "    hybrid"
    echo "    debug-uni"
    echo "    hybrid-uni"
    echo "    both"
    echo "    both-uni"
    echo "    both-hybrid"
    echo "    all"
    echo "    release"
    echo "    release-uni"
    exit 1
fi


echo ------------------
echo cd $WXWIN/build/msw
cd $WXWIN/build/msw
nmake -f .makesetup.mk $WXWIN/lib/vc_dll/$WXLIBDIR/wx/setup.h
if [ ! $? = 0 ]; then error 2; fi

echo ------------------
echo cd $WXWIN/build/msw
cd $WXWIN/build/msw
$WXWIN/build/msw/.mymake $BUILD_TYPE $@
if [ ! $? = 0 ]; then error 2; fi


echo ------------------
echo cd ../../contrib/build/gizmos
cd ../../contrib/build/gizmos
$WXWIN/build/msw/.mymake $BUILD_TYPE $@
if [ ! $? = 0 ]; then error 2; fi
cd -


# echo ------------------
# echo cd ../../contrib/build/animate
# cd ../../contrib/build/animate
# $WXWIN/build/msw/.mymake $BUILD_TYPE $@
# if [ ! $? = 0 ]; then error 2; fi
# cd -


echo ------------------
echo cd ../../contrib/build/stc
cd ../../contrib/build/stc
$WXWIN/build/msw/.mymake $BUILD_TYPE $@
if [ ! $? = 0 ]; then error 2; fi
cd -


# echo ------------------
# echo cd ../../contrib/build/ogl
# cd ../../contrib/build/ogl
# $WXWIN/build/msw/.mymake $BUILD_TYPE CPPFLAGS="-DwxUSE_DEPRECATED=0" $@
# if [ ! $? = 0 ]; then error 2; fi
# cd -



echo -----------------
echo --   SUCCESS!  --
echo -----------------
# copy DLLs to a dir on the PATH
# copy /U $WXWIN/lib/vc_dll/*.dll $WXWIN/BIN
# copy /U $WXWIN/lib/vc_dll/*.pdb $WXWIN/BIN
exit 0





#!/bin/sh

# Make a distribution of ConfigTool for Linux
# Usage: maketarball.sh wxconfigtool-dir bin-dir deliver-dir version [ options ]
# For example: maketarball.sh ~/wxconfigtool /tmp/build-wxconfigtool /tmp/wxconfigtool-deliver 1.40

APPDIR=$1
SRC=$APPDIR/src
BINARYSRC=$2
DEST=$3
BUILD=0
UPX=0
PROGNAME=$0

# Set this to the required version
VERSION=$4

dotar()
{
    rm -f -r $DEST/wxconfigtool*
    rm -f $DEST/wxconfigtool-*.*

    mkdir -p $DEST/wxconfigtool-$VERSION
    mkdir -p $DEST/wxconfigtool-$VERSION/resources
    mkdir -p $DEST/wxconfigtool-$VERSION/Sample
    cd $DEST/wxconfigtool-$VERSION

	# Copy readme files
    cp $APPDIR/docs/readme.txt readme.txt
    cp $APPDIR/docs/license.txt .

	# Copy the application binary
    cp $BINARYSRC/wxconfigtool .

	# Copy the wxHTML Help manual file
    cp $APPDIR/manual/configtool.htb .

	# Copy the sample
    cp -r $APPDIR/sample Sample
    cp $APPDIR/resources/* resources

    echo Removing junk from the samples folder...
    rm -f -r Sample/CVS

	# Copy the resources file
    rm -f configtool.bin
    zip configtool.bin -j resources/*

    rm -f -r resources

	# Remove any debug info from wxconfigtool
    strip wxconfigtool

	# Maybe compress the binary
    if [ "$UPX" != "0" ]; then
        upx wxconfigtool
    fi

    cd ..
    
	# Make .tar.gz and .tar.bz2 archives
    tar cvf $DEST/wxconfigtool-$VERSION-i386.tar wxconfigtool-$VERSION/*
    gzip -c $DEST/wxconfigtool-$VERSION-i386.tar > $DEST/wxconfigtool-$VERSION-i386.tar.gz
    bzip2 -c $DEST/wxconfigtool-$VERSION-i386.tar > $DEST/wxconfigtool-$VERSION-i386.tar.bz2
}

dobuild()
{
    makeapp release full
}

usage()
{
    echo Usage: $PROGNAME "cvs-dir bin-dir deliver-dir version-number [ options ]"
    echo Options:
    echo "    --help         Display this help message"
    echo "    --upx          Compress executable with UPX"
    echo "    --build        Invoke 'maketarball.sh release full' first"
	echo For example: maketarball.sh ~/wxconfigtool /tmp/build-app /tmp/wxconfigtool-deliver 1.20
    exit 1
}

# Process command line options.
shift 4
for i in "$@"; do
    case "$i" in
	--build) BUILD=1 ;;
	--upx) UPX=1 ;;
	*)
	    usage
	    exit
	    ;;
    esac
done

if [ ! -d "$DEST" ]; then
    mkdir -p $DEST
fi

if [ ! -d "$SRC" ]; then
    echo Source directory $SRC not found.
    usage
    exit 1
fi

if [ ! -d "$BINARYSRC" ]; then
    echo Location of wxconfigtool binary $BINARYSRC not found.
    usage
    exit 1
fi

if [ "$VERSION" = "" ]; then
    echo Pass the version number as the fourth argument.
    usage
    exit 1
fi

echo Creating Version $VERSION distribution in $DEST, using source directory $SRC and wxconfigtool binary in $BINARYSRC.
#echo Press return to continue.
#read dummy

# Remove all existing files
if [ ! -d "$DEST/wxconfigtool" ]; then
    rm -f -r $DEST/wxconfigtool
fi

# Skip INNO setup if INNO is 0.
if [ "$BUILD" = "1" ]; then
    dobuild
fi

dotar

echo ConfigTool archived.


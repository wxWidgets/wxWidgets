#!/bin/sh

# Make a distribution of Tex2RTF for Unix
# Julian Smart 2002-08-07
# Usage: maketarball.sh wx-dir bin-file deliver-dir version-no [ options ]
# For example: maketarball.sh ~/wx2dev/wxWindows /bin/tex2rtf /tmp/tex2rtf-deliver
# Where:
# wx-dir is the wxWindows source tree
# bin-file is the Tex2RTF binary, already compiled
# deliver-dir is the directory the distribution will be put in
# version-no is the version, e.g. 2.01

TEX2RTFDIR=$1/utils/tex2rtf
SRC=$TEX2RTFDIR/src
BINARYSRC=$2
DEST=$3
BUILD=0
UPX=0
PROGNAME=$0
VERSION=$4

dotar()
{
    # Make the documentation first
    cd $TEX2RTFDIR/docs
    make htb

    rm -f -r $DEST/tex2rtf-*.*

    mkdir -p $DEST/tex2rtf-$VERSION
    mkdir -p $DEST/tex2rtf-$VERSION/tools
    mkdir -p $DEST/tex2rtf-$VERSION/docs
    mkdir -p $DEST/tex2rtf-$VERSION/html

    # Make the source archive
    cd $TEX2RTFDIR
    ls `cat $TEX2RTFDIR/distrib/src.rsp` > /tmp/tex2rtf.txt
    tar cvf $DEST/tex2rtf-$VERSION/tex2rtf-$VERSION-source.tar -T /tmp/tex2rtf.txt
    gzip $DEST/tex2rtf-$VERSION/tex2rtf-$VERSION-source.tar
    rm /tmp/tex2rtf.txt

    cd $DEST/tex2rtf-$VERSION

    # Copy the binary and other files
    cp $BINARYSRC tex2rtf
    cp $TEX2RTFDIR/docs/readme.txt readme.txt
    cp $TEX2RTFDIR/docs/licence.txt .
    cp $TEX2RTFDIR/docs/gpl.txt .
    cp $TEX2RTFDIR/docs/lgpl.txt .
    cp $TEX2RTFDIR/docs/*.html $TEX2RTFDIR/docs/*.gif html
    cp $TEX2RTFDIR/docs/tex2rtf.htb .

    # Copy all the doc sources, so that the user can experiment
    # on the Tex2RTF docs 
    cp $TEX2RTFDIR/docs/*.tex $TEX2RTFDIR/docs/*.ini $TEX2RTFDIR/docs/*.sty $TEX2RTFDIR/docs/*.bib $TEX2RTFDIR/docs/*.gif $TEX2RTFDIR/docs/*.tex docs

    strip tex2rtf

    if [ "$UPX" != "0" ]; then
        upx tex2rtf
    fi

    cd ..
    
    tar cvf $DEST/tex2rtf-$VERSION-i386.tar tex2rtf-$VERSION/*
    gzip -c $DEST/tex2rtf-$VERSION-i386.tar > $DEST/tex2rtf-$VERSION-i386.tar.gz
    bzip2 -c $DEST/tex2rtf-$VERSION-i386.tar > $DEST/tex2rtf-$VERSION-i386.tar.bz2
}

usage()
{
    echo Usage: $PROGNAME "wx-dir bin-file deliver-dir version-no [ options ]"
    echo Options:
    echo "    --help         Display this help message"
    echo "    --upx          Compress executable with UPX"
    echo For example: $PROGNAME ~/wx2dev/wxWindows /tmp/tex2rtf /tmp/tex2rtf-deliver 2.0
    exit 1
}

# Process command line options.
shift 4
for i in "$@"; do
    case "$i" in
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

if [ ! -f "$BINARYSRC" ]; then
    echo tex2rtf binary $BINARYSRC not found.
    usage
    exit 1
fi

if [ "$VERSION" = "" ]; then
    echo Pass the version number as the fourth argument.
    usage
    exit 1
fi

echo Creating Version $VERSION distribution in $DEST, using source directory $SRC and tex2rtf binary $BINARYSRC.

dotar

echo Tex2RTF archived.


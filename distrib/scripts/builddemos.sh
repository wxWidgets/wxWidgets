#!/bin/sh

# Build a range of Unix demos for uploading to the wxWidgets web site.
# Example:
# builddemos.sh --wxmsw --verbose &> log

PROGNAME=$0
WXSRCDIR=$WXWIN
WXDESTDIR=$WXSRCDIR/deliver

# Suffix to append to each demo tarball, e.g. SuSE92 for widgets-SuSE92
SUFFIX=linux

dobuilddemos()
{
    if [ ! -d "$WXSRCDIR" ] ; then
        echo Cannot find wxWidgets source directory. Use --wxdir option.
        exit
    fi
    if [ ! -d "$WXDESTDIR" ] ; then
        echo Cannot find wxWidgets destination directory. Use --deliver option.
        exit
    fi
    if [ ! -d samples ] ; then
        echo Cannot find samples directory. Invoke this script from the root of the build folder.
        exit
    fi
    if [ -d src ] ; then
        echo Do not invoke this from the source directory - invoke from the build directory.
        exit
    fi

    echo wxWidgets build directory is `pwd`
    echo wxWidgets source directory is $WXSRCDIR
    echo wxWidgets deliver directory is $WXDESTDIR
    echo Suffix is $SUFFIX

    rm -f $WXDESTDIR/*.tar.gz

    # Dialogs

    pushd samples/dialogs
    make
    if [ -f dialogs ] ; then
        cp $WXSRCDIR/samples/dialogs/*.cpp $WXSRCDIR/samples/dialogs/*.h .
        tar cfz $WXDESTDIR/wxWidgets-DialogsDemo-$SUFFIX.tar.gz dialogs *.cpp *.h
        rm -f *.cpp *.h
    else
        echo "*** Warning: dialogs sample did not build"
    fi
    popd
    
    # HTML

    pushd samples/html/test
    make
    if [ -f test ] ; then
        mv test htmldemo
        cp $WXSRCDIR/samples/html/test/*.cpp $WXSRCDIR/samples/html/test/*.htm* $WXSRCDIR/samples/html/test/*.png $WXSRCDIR/samples/html/test/*.gif .
        tar cfz $WXDESTDIR/wxWidgets-HtmlDemo-$SUFFIX.tar.gz htmldemo *.cpp *.png *.gif *.htm*
        rm -f *.cpp *.gif *.png *.htm*
    else
        echo "*** Warning: HTML sample did not build"
    fi
    popd
    
    # Widgets

    pushd samples/widgets
    make
    if [ -f widgets ] ; then
        cp $WXSRCDIR/samples/widgets/*.cpp $WXSRCDIR/samples/widgets/*.h .
        tar cfz $WXDESTDIR/wxWidgets-WidgetsDemo-$SUFFIX.tar.gz widgets *.cpp *.h
        rm -f *.cpp *.h
    else
        echo "*** Warning: widgets sample did not build"
    fi
    popd
    
    # Life

    pushd demos/life
    make
    if [ -f life ] ; then
        cp $WXSRCDIR/demos/life/*.cpp $WXSRCDIR/demos/life/*.h $WXSRCDIR/demos/life/*.xpm $WXSRCDIR/demos/life/*.inc $WXSRCDIR/demos/life/*.lif .
        mkdir bitmaps
        cp $WXSRCDIR/demos/life/bitmaps/*.* bitmaps
        tar cfz $WXDESTDIR/wxWidgets-LifeDemo-$SUFFIX.tar.gz life *.cpp *.h *.xpm *.inc *.lif bitmaps/*.*
        rm -f *.cpp *.h *.xpm *.inc *.lif
        rm -f -r bitmaps
    else
        echo "*** Warning: life demo did not build"
    fi
    popd    
}

usage()
{
    echo "Usage: $PROGNAME [ options ]" 1>&2
    echo Options:
    echo "    --help            Display this help message"
    echo "    --suffix          Append suffix to each tarball"
    echo "    --wxdir           wxWidgets source dir"
    echo "    --deliver         Tarball destination dir"
    echo.
    echo Note that options only override settings in $SCRIPTDIR/setup.var.
    exit 1
}

# Process command line options.

while [ $# -gt 0 ]
do
    case "$1" in
        --suffix) SUFFIX=$2; shift;;
        --wxdir) WXSRCDIR=$2; WXDESTDIR=$WXSRCDIR/deliver; shift;;
        --deliver) WXDESTDIR=$2; shift;;
        *)
            usage
	        exit
	        ;;
    esac
    shift
done

dobuilddemos


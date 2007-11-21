#!/bin/sh

# Build a range of Unix demos for uploading to the wxWidgets web site.
# Example:
# builddemos.sh --wxmsw --verbose &> log

PROGNAME=$0
WXSRCDIR=$WXWIN
WXDESTDIR=$WXSRCDIR/deliver
PLATFORM=linux

# Default binary subdirectory to copy sample executable from
WINBINDIR=vc_msw

# Suffix to append to each demo tarball, e.g. SuSE92 for widgets-SuSE92
SUFFIX=linux

# Determine if the binary name is .app or not
determine_binary()
{
    if [ "$PLATFORM" = "mac" ]; then
        BINARYNAME=$1.app
    else
        BINARYNAME=$1
    fi
}

dobuilddemos_unix()
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
    if [ -d include ] ; then
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
        determine_binary dialogs
        cp $WXSRCDIR/samples/dialogs/*.cpp $WXSRCDIR/samples/dialogs/*.h .
        tar cfz $WXDESTDIR/wxWidgets-DialogsDemo-$SUFFIX.tar.gz $BINARYNAME *.cpp *.h
        rm -f *.cpp *.h
    else
        echo "*** Warning: dialogs sample did not build"
    fi
    popd
    
    # HTML

    pushd samples/html/test
    make
    if [ -f test ] ; then
        if [ "$PLATFORM" = "mac" ]; then
            # Don't rename app on Mac in case it messes up the bundle
            determine_binary test
        else
            mv test htmldemo
            determine_binary htmldemo
        fi
        cp $WXSRCDIR/samples/html/test/*.cpp $WXSRCDIR/samples/html/test/*.htm* $WXSRCDIR/samples/html/test/*.png $WXSRCDIR/samples/html/test/*.gif .
        tar cfz $WXDESTDIR/wxWidgets-HtmlDemo-$SUFFIX.tar.gz $BINARYNAME *.cpp *.png *.gif *.htm*
        rm -f *.cpp *.gif *.png *.htm*
    else
        echo "*** Warning: HTML sample did not build"
    fi
    popd
    
    # Widgets

    pushd samples/widgets
    make
    if [ -f widgets ] ; then
        determine_binary widgets
        cp $WXSRCDIR/samples/widgets/*.cpp $WXSRCDIR/samples/widgets/*.h .
        tar cfz $WXDESTDIR/wxWidgets-WidgetsDemo-$SUFFIX.tar.gz $BINARYNAME *.cpp *.h
        rm -f *.cpp *.h
    else
        echo "*** Warning: widgets sample did not build"
    fi
    popd
    
    # Life

    pushd demos/life
    make
    if [ -f life ] ; then
        determine_binary life
        cp $WXSRCDIR/demos/life/*.cpp $WXSRCDIR/demos/life/*.h $WXSRCDIR/demos/life/*.xpm $WXSRCDIR/demos/life/*.inc $WXSRCDIR/demos/life/*.lif .
        mkdir bitmaps
        cp $WXSRCDIR/demos/life/bitmaps/*.* bitmaps
        tar cfz $WXDESTDIR/wxWidgets-LifeDemo-$SUFFIX.tar.gz $BINARYNAME *.cpp *.h *.xpm *.inc *.lif bitmaps/*.*
        rm -f *.cpp *.h *.xpm *.inc *.lif
        rm -f -r bitmaps
    else
        echo "*** Warning: life demo did not build"
    fi
    popd    
}

dobuilddemos_windows()
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

    echo wxWidgets build directory is `pwd`
    echo wxWidgets source directory is $WXSRCDIR
    echo wxWidgets deliver directory is $WXDESTDIR
    echo Suffix is $SUFFIX

    rm -f $WXDESTDIR/*.zip

    # Dialogs

    pushd samples/dialogs
    if [ -f $WINBINDIR/dialogs.exe ] ; then
        cp $WINBINDIR/dialogs.exe .
        zip $WXDESTDIR/wxWidgets-DialogsDemo-$SUFFIX.zip dialogs.exe *.cpp *.h
        rm -f dialogs.exe
    else
        echo "*** Warning: dialogs sample did not build"
    fi
    popd
    
    # HTML

    pushd samples/html/test
    if [ -f $WINBINDIR/test.exe ] ; then
        cp $WINBINDIR/test.exe htmldemo.exe
        zip $WXDESTDIR/wxWidgets-HtmlDemo-$SUFFIX.zip htmldemo.exe *.cpp *.png *.gif *.htm*
        rm -f htmldemo.exe
    else
        echo "*** Warning: HTML sample did not build"
    fi
    popd
    
    # Widgets

    pushd samples/widgets
    if [ -f $WINBINDIR/widgets.exe ] ; then
        cp $WINBINDIR/widgets.exe .
        zip $WXDESTDIR/wxWidgets-WidgetsDemo-$SUFFIX.zip widgets.exe *.cpp *.h
    else
        echo "*** Warning: widgets sample did not build"
    fi
    popd
    
    # Life

    pushd demos/life
    if [ -f $WINBINDIR/life.exe ] ; then
        cp $WINBINDIR/life.exe .
        zip $WXDESTDIR/wxWidgets-LifeDemo-$SUFFIX.zip life.exe *.cpp *.h *.xpm *.inc *.lif bitmaps/*.*
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
    echo "    --mac             Whether we're building on Mac"
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
        --mac) PLATFORM=mac;;
        *)
            usage
	        exit
	        ;;
    esac
    shift
done

if [ "$OS" = "Windows_NT" ]; then
    dobuilddemos_windows
else
    dobuilddemos_unix
fi




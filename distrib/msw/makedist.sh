#! /bin/sh
# makedist.sh
#
# Build wxWindows 2 for Windows distribution.
# This builds all required binaries and documents before calling
# zipdist.sh to make the archives.
#
# To use this script, you need:
#
# - CygWin installation, for bash etc.
# - VC++ 6 or higher, to compile the binaries
# - WinHelp compiler, HTML Help compiler, Tex2RTF on your path
# - WISE Install 5
#
# Julian Smart, October 2000

SRC=`cygpath -u $WXWIN`
DEST=$SRC/deliver
TMPDIR=`cygpath -u $TEMP`
OK=1
DOWISE=0
# For some reason, if we pipe output to egrep, we see output, but not otherwise.
WARNINGS=": decorated name|: see reference|: see declaration|C4786|VC98\\\\INCLUDE|template<>"

setup_vars() {
    VCPATH="/c/Program Files/Microsoft Visual Studio/common/msdev98/bin:/c/Program Files/Microsoft Visual Studio/VC98/bin:DevStudio/VC/bin:/c/Program Files/Microsoft Visual Studio/common/tools:/c/Program Files/HTML Help Workshop"
    INCLUDE="C:\Program Files\Microsoft Visual Studio\VC98\ATL\INCLUDE;C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE;C:\Program Files\Microsoft Visual Studio\VC98\MFC\INCLUDE;C:\Program Files\Tcl\include;C:\Program Files\HTML Help Workshop\include"
    LIB="C:\Program Files\Microsoft Visual Studio\VC98\lib;C:\Program Files\Microsoft Visual Studio\VC98\MFC\lib;C:\Program Files\Tcl\lib;C:\Program Files\HTML Help Workshop\lib"
    TCLHOME=C:/PROGRA~1/Tcl export TCLHOME
    PATH="$PATH:$VCPATH" export PATH
    export INCLUDE LIB
}

check_compile() {
    egrep ": error C|fatal error" $TMPDIR/buildlog.txt > $TMPDIR/errorlog.txt
    if [ -s $TMPDIR/errorlog.txt ]; then
        echo Did not build $0 successfully.
        OK=0
    fi
}

check_files() {
    if [ ! -d "$SRC" ]; then
        echo "$SRC" does not exist.
        OK=0
    fi

    if [ ! -d "$SRC/deliver" ]; then
        mkdir "$SRC/deliver"
    fi

    if [ ! -e $SRC/include/wx/msw/setup.h ]; then
        cp "$SRC/include/wx/msw/setup0.h" "$SRC/include/wx/msw/setup.h"
        echo setup0.h has been copied to setup.h.
        echo You must now edit this file to restore release settings,
        echo then run this script again.
        OK=0
        notepad.exe "$SRC/include/wx/msw/setup.h"
    fi
    if [ ! -d "$SRC/bin" ]; then
        mkdir "$SRC/bin"
    fi
    if [ ! -e "$SRC/bin/DBVIEW.EXE" ]; then
        echo Please put DBGVIEW.EXE, DBGVIEW.CNT, DBGVIEW.HLP into $SRC/bin
        echo and run the script again.
        OK=0
    fi
}

build_docs() {
    cd "$SRC/src/msw"
    echo "---------------------------------"
    echo "Building wxWindows documents"
    nmake -f makefile.vc cleandocs docs

    cd "$SRC/utils/dialoged/src"
    nmake -f makefile.vc html htmlhelp htb hlp pdfrtf

    cd "$SRC/utils/tex2rtf/src"
    nmake -f makefile.vc html htmlhelp htb hlp pdfrtf

    cd "$SRC/contrib/src/ogl"
    nmake -f makefile.vc html htmlhelp htb hlp pdfrtf

    cd "$SRC/contrib/src/mmedia"
    # NB: add htb target
    nmake -f makefile.vc html htmlhelp hlp pdfrtf
}

# Build wxWindows
build_wxwin_vc() {
    echo "---------------------------------"
    echo "Building wxWindows using VC++"
    cd "$SRC/src"
    echo Building wxWindows Release library in `pwd`
    echo Command: msdev wxvc.dsw /useenv /make "wxvc - Win32 Release" /rebuild
    msdev wxvc.dsw /useenv /make "wxvc - Win32 Release" /rebuild | egrep -v "$WARNINGS"
}

build_dialog_editor() {
    echo "---------------------------------"
    echo "Building Dialog Editor using VC++"
    cd "$SRC/utils/dialoged/src"
    msdev DialogEdVC.dsw /useenv /make "DialogEdVC - Win32 Release" /rebuild | egrep -v "$WARNINGS" | tee $TMPDIR/buildlog.txt

    check_compile "Dialog Editor"
}

build_tex2rtf() {
    echo "---------------------------------"
    echo "Building Tex2RTF using VC++"
    cd "$SRC/utils/tex2rtf/src"
    msdev Tex2RTFVC.dsw /useenv /make "Tex2RTFVC - Win32 Release" /rebuild | egrep -v "$WARNINGS" | tee $TMPDIR/buildlog.txt

    check_compile "Tex2RTF"
}

build_life() {
    echo "---------------------------------"
    echo "Building Life! using VC++"
    cd "$SRC/demos/life"
    msdev LifeVC.dsw /useenv /make "LifeVC - Win32 Release" /rebuild | egrep -v "$WARNINGS" | tee $TMPDIR/buildlog.txt

    check_compile "Life! Demo"
}

build_executables() {
    build_dialog_editor
    build_tex2rtf
    build_life
}

copy_files() {
    cp "$SRC/utils/dialoged/src/Release/dialoged.exe" "$SRC/bin"
    cp "$SRC/docs/winhelp/dialoged.*" "$SRC/bin"

    cp "$SRC/utils/tex2rtf/src/Release/tex2rtf.exe" "$SRC/bin"
    cp "$SRC/docs/winhelp/tex2rtf.*" "$SRC/bin"

    cp "$SRC/demos/life/life.exe" "$SRC/demos/life/breeder.lif" "$SRC/bin"
}

# Process command line options.
for i in "$@"; do
    case "$i" in
	--wise) DOWISE=1 ;;
	*)
	    echo Usage: $0 "[ options ]"
        echo Generates documentation and binaries for creating a distribution,
        echo and optionally generates the zip/setup.exe distribution by
        echo calling zipdist.sh.
        echo
	    echo Options:
	    echo "    --help         Display this help message"
	    echo "    --wise         Build zips and setup.exe after creating docs and binaries"
	    exit 1
	    ;;
    esac
done

mkdir -p $SRC/docs/pdf
mkdir -p $SRC/docs/html
mkdir -p $SRC/docs/htmlhelp
mkdir -p $SRC/docs/htb

setup_vars
check_files

if [ "$OK" = "1" ]; then
    build_docs
fi

if [ "$OK" = "1" ]; then
    build_wxwin_vc
fi

if [ "$OK" = "1" ]; then
    build_executables
fi

if [ "$OK" = "1" ]; then
    copy_files
fi

if [ "$OK" = "1" && "$DOWISE" = "1" ]; then
    $SRC/distrib/msw/zipdist.sh --wise
fi

if [ "$OK" = "1" ]; then
    echo Finished successfully.
else
    echo Finished unsuccessfully. There were errors.
fi

echo Press return to continue.
read dummy


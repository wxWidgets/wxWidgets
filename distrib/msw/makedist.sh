#! /bin/bash
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
# - Word 97 (not tested with higher versions)
# - Adobe Acrobat & Distiller
#
# Before running this script, you will need to:
#
# - update the readmes, change log, manual version etc.
# - update version.h
# - update distrib/msw/wisetop.txt, wisebott.txt with the correct version
#   number, plus any hard-wired wxWindows paths
# - test on a variety of compilers
#
# TODO:
#
# - generation of PDF (only PDF RTF generated so far)
# - perhaps prompt the user to read the important release docs,
#   version.h, setup.h
#
# Julian Smart, October 2000

SRC=`cygpath -u $WXWIN`
DEST=$SRC/deliver
TMPDIR=`cygpath -u $TEMP`
OK=1
DOWISE=0
DOPDF=0
DOALL=1
DOCSONLY=0
WXWINONLY=0
WISEONLY=0
BINONLY=0
PDFONLY=0

# For some reason, if we pipe output to egrep, we see output, but not otherwise.
WARNINGS=": decorated name|: see reference|: see declaration|C4786|VC98\\\\INCLUDE|template<>"

setup_vars() {
    VCPATH="/c/Program Files/Microsoft Visual Studio/common/msdev98/bin:/c/Program Files/Microsoft Visual Studio/VC98/bin:DevStudio/VC/bin:/c/Program Files/Microsoft Visual Studio/common/tools:/c/Program Files/HTML Help Workshop"
    INCLUDE="C:\Program Files\Microsoft Visual Studio\VC98\ATL\INCLUDE;C:\Program Files\Microsoft Visual Studio\VC98\INCLUDE;C:\Program Files\Microsoft Visual Studio\VC98\MFC\INCLUDE;C:\Program Files\Tcl\include;C:\Program Files\HTML Help Workshop\include"
    LIB="C:\Program Files\Microsoft Visual Studio\VC98\lib;C:\Program Files\Microsoft Visual Studio\VC98\MFC\lib;C:\Program Files\Tcl\lib;C:\Program Files\HTML Help Workshop\lib"
    TCLHOME=C:/PROGRA~1/Tcl export TCLHOME
    PATH="$PATH:$VCPATH" export PATH
    export INCLUDE LIB

    WORDEXE="/c/Program Files/Microsoft Office/Office/WINWORD.EXE"
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
    if [ ! -e "$SRC/bin/DBGVIEW.EXE" ]; then
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
    nmake -f makefile.vc html htmlhelp htb hlp pdfrtf
}

# TODO: Make PDF via Word, if Word and Adobe Acrobat are present.
# This has to be interactive at present.
build_pdf() {
    echo "---------------------------------"
    echo "Building wxWindows PDF documents"
    if [ -e "$WORDEXE" ]; then
        "$WORDEXE" "$WXWIN\\docs\\pdf\\wx.rtf"
        "$WORDEXE" "$WXWIN\\docs\\pdf\\dialoged.rtf"
        "$WORDEXE" "$WXWIN\\docs\\pdf\\tex2rtf.rtf"
        "$WORDEXE" "$WXWIN\\contrib\\docs\\pdf\\ogl.rtf"
        "$WORDEXE" "$WXWIN\\contrib\\docs\\mmedia\\ogl.rtf"
    else
        echo MS Word not present. Not doing PDF build.
    fi
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
    cp "$SRC/docs/winhelp/dialoged.hlp" "$SRC/docs/winhelp/dialoged.cnt" "$SRC/bin"

    cp "$SRC/utils/tex2rtf/src/Release/tex2rtf.exe" "$SRC/bin"
    cp "$SRC/docs/winhelp/tex2rtf.hlp" "$SRC/docs/winhelp/tex2rtf.cnt" "$SRC/bin"

    cp "$SRC/demos/life/Release/life.exe" "$SRC/demos/life/breeder.lif" "$SRC/bin"
}

# Process command line options.
for i in "$@"; do
    case "$i" in
	--wise) DOWISE=1 ;;
	--pdf) DOPDF=1 ;;
	--wise-only)
        WISEONLY=1
        DOWISE=1
        DOALL=0
        ;;
	--docs-only)
        DOCSONLY=1
        DOALL=0
        ;;
	--bin-only)
        BINONLY=1
        DOALL=0
        ;;
	--wxwin-only)
        WXWINONLY=1
        DOALL=0
        ;;
	--pdf-only)
        PDFONLY=1
        DOPDF=1
        DOALL=0
        ;;
	*)
	    echo Usage: $0 "[ options ]"
        echo Generates documentation and binaries for creating a distribution,
        echo and optionally generates the zip/setup.exe distribution by
        echo calling zipdist.sh.
        echo
	    echo Options:
	    echo "    --help         Display this help message"
	    echo "    --wise         Additonally, build zips and setup.exe"
	    echo "    --pdf          Additionally, try to generate PDF"
	    echo "    --wise-only    Only do zip/setup phase"
	    echo "    --wxwin-only   Only do wxWin lib building phase"
	    echo "    --docs-only    Only do docs building phase"
	    echo "    --pdf-only     Only do PDF building phase"
	    echo "    --bin-only     Only do .exe building phase"
	    exit 1
	    ;;
    esac
done

mkdir -p $SRC/docs/pdf
mkdir -p $SRC/docs/html
mkdir -p $SRC/docs/htmlhelp
mkdir -p $SRC/docs/htb
mkdir -p $SRC/docs/winhelp
mkdir -p $SRC/contrib/docs/pdf
mkdir -p $SRC/contrib/docs/html
mkdir -p $SRC/contrib/docs/htmlhelp
mkdir -p $SRC/contrib/docs/htb
mkdir -p $SRC/contrib/docs/winhelp

setup_vars
check_files

if [ "$OK"  = "1" ]; then
    if [ "$DOCSONLY" = "1" ] || [ "$DOALL" = "1" ]; then
        build_docs
    fi
fi

if [ "$OK" = "1" ] && [ "$DOPDF" = "1" ]; then
    if [ "$PDFONLY" = "1" ] || [ "$DOALL" = "1" ]; then
        build_pdf
    fi
fi

if [ "$OK" = "1" ]; then
    if [ "$WXWINONLY" = "1" ] || [ "$DOALL" = "1" ]; then
        build_wxwin_vc
    fi
fi

if [ "$OK" = "1" ]; then
    if [ "$BINONLY" = "1" ] || [ "$DOALL" = "1" ]; then
        build_executables
    fi
fi

if [ "$OK" = "1" ]; then
    copy_files
fi

if [ "$OK" = "1" ] && [ "$DOWISE" = "1" ]; then
    if [ "$WISEONLY" = "1" ] || [ "$DOALL" = "1" ]; then
        $SRC/distrib/msw/zipdist.sh --wise
    fi
fi

if [ "$OK" = "1" ]; then
    echo Finished successfully.
else
    echo Finished unsuccessfully. There were errors.
fi

echo Press return to continue.
read dummy


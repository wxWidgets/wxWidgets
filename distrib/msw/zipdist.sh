#!/bin/sh

# Zip up an external, generic + Windows distribution of wxWindows 2
SRC=`cygpath -u $WXWIN`
DEST=$SRC/deliver
WISE=0
WISEONLY=0
# If your zip accepts Cygwin-style paths, then
# use cygpath, else substitute echo
CYGPATHPROG=cygpath
#CYGPATHPROG=echo

# Set this to the required version
VERSION=2.3.0

dowise()
{
    cd $DEST

    # Unzip the Windows files into 'wx'
    mkdir $DEST/wx

    # After this change of directory, we're in the
    # temporary 'wx' directory and not acting on
    # the source wxWindows directory.
    cd $DEST/wx
    unzip -o `$CYGPATHPROG -w ../wxWindows-$VERSION-msw.zip`
    unzip -o `$CYGPATHPROG -w ../wxWindows-$VERSION-gen.zip`
    unzip -o `$CYGPATHPROG -w ../wxWindows-$VERSION-vc.zip`
    unzip -o `$CYGPATHPROG -w ../wxWindows-$VERSION-bc.zip`
    unzip -o `$CYGPATHPROG -w ../wxWindows-$VERSION-HTMLHelp.zip`
    unzip -o `$CYGPATHPROG -w ../extradoc.zip`
    # Need Word file, for Remstar DB classes
    unzip -o `$CYGPATHPROG -w ../wxWindows-$VERSION-Word.zip`
    unzip -o `$CYGPATHPROG -w ../ogl3.zip`
    unzip -o `$CYGPATHPROG -w ../jpeg.zip`
    unzip -o `$CYGPATHPROG -w ../tiff.zip`
    unzip -o `$CYGPATHPROG -w ../tex2rtf2.zip`

    # Now delete a few files that are unnecessary
    # attrib -R *
    rm -f BuildCVS.txt *.in *.spec *.guess *.sub mkinstalldirs modules install-sh *.sh descrip.mms
    rm -f configure samples/configure samples/*.in demos/configure demos/*.in contrib/configure contrib/*.in
    rm -f setup.h.in setup.h_vms
    rm -f -r Makefile.in
    rm -f docs/html/wxbook.htm docs/html/roadmap.htm
    rm -f contrib/docs/winhelp/mmedia.*
    rm -f contrib/docs/winhelp/stc.*
    rm -f contrib/docs/htmlhelp/mmedia.*
    rm -f contrib/docs/htmlhelp/stc.*
    rm -f contrib/docs/pdf/*.*
    rm -f -r contrib/docs/latex/ogl
    rm -f SRC/mingegcs.bat
    rm -f distrib

    # Now copy some binary files to 'bin'
    mkdir bin
    cp $SRC/bin/dialoged.exe bin
    cp $SRC/bin/tex2rtf.exe bin
    cp $SRC/bin/dbgview.* bin
    cp $SRC/bin/life.exe bin
    cp $SRC/docs/winhelp/dialoged.hlp $SRC/docs/winhelp/dialoged.cnt bin
    cp $SRC/docs/winhelp/tex2rtf.hlp $SRC/docs/winhelp/tex2rtf.cnt bin

    # Make wxMSW-xxx.zip
    zip -r `$CYGPATHPROG -w ../wxMSW-$VERSION.zip` *

    # Time to regenerate the WISE install script, wxwin2.wse.
    # NB: if you've changed wxwin2.wse using WISE, call splitwise.exe
    # from within distrib/msw, to split off wisetop.txt and wisebott.txt.
    echo Calling 'makewise' to generate wxwin2.wse...

    sh $WXWIN/distrib/msw/makewise.sh

    rm -f $DEST/setup.*

    # Now invoke WISE install on the new wxwin2.wse
    echo Invoking WISE...
    /c/progra~1/wise/wise32.exe /C $WXWIN\\distrib\\msw\\wxwin2.wse
    echo Press return to continue with the wxWindows distribution...
    read dummy

    cd $DEST

    # tidy up capitalisation of filenames
    mv setup.EXE s
    mv s setup.exe

    mv setup.w02 s
    mv s setup.w02

    mv setup.w03 s
    mv s setup.w03

    mv setup.w04 s
    mv s setup.w04

    mv setup.w05 s
    mv s setup.w05

    mv setup.w06 s
    mv s setup.w06

    mv setup.w07 s
    mv s setup.w07

    mv setup.w08 s
    mv s setup.w08

    mv setup.w09 s
    mv s setup.w09

    # Put all the setup files into a single zip archive.
    zip wxMSW-$VERSION-setup.zip readme.txt setup.*
}

expandlines()
{
    toexpand=$1
    outputfile=$2

    rm -f $outputfile
    touch $outputfile
    for line in `cat $toexpand` ; do
      if [ $line != "" ]; then
        ls $line >> $outputfile
      fi
    done
}

# Process command line options.
for i in "$@"; do
    case "$i" in
	--wise) WISE=1 ;;
	--wiseonly) WISEONLY=1 ;;
	*)
	    echo Usage: $0 "[ options ]" 1>&2
	    echo Options:
	    echo "    --help         Display this help message"
	    echo "    --wise         Also build setup.exe and wxMSW-version.zip"
	    echo "    --wiseonly     Build setup.exe and wxMSW-version.zip only"
	    exit 1
	    ;;
    esac
done

if [ ! -d "$SRC" ]; then
    echo $SRC not found.
    exit 1
fi

if [ ! -d "$DEST" ]; then
    echo $DEST not found.
    exit 1
fi

echo Creating distribution in $DEST. Press return to continue.
read dummy

if [ "$WISEONLY" != "0" ]; then
        dowise
        exit 0
fi

# Remove all existing files
rm -f  $DEST/wx*.zip
rm -f  $DEST/*.htb
rm -f  $DEST/ogl3.zip
rm -f  $DEST/tex2rtf2.zip
rm -f  $DEST/jpeg.zip
rm -f  $DEST/tiff.zip
rm -f  $DEST/dialoged.zip
rm -f  $DEST/utils.zip
rm -f  $DEST/extradoc.zip
rm -f  $DEST/*-win32.zip

if [ ! -d "$DEST/wx" ]; then
    rm -f -r $DEST/wx
fi


cd $SRC
echo Zipping...

# Below is the old-style separated-out format. This is retained only
# for local use, and for creating wxMSW-xxx.zip.

# We can't use e.g. this:
# ls `cat $SRC/distrib/msw/makefile.rsp` zip -@ -u $DEST/wxWindows-$VERSION-gen.zip
# because there's not enough space on the command line, plus we need to ignore the
# blank lines.

expandlines $SRC/distrib/msw/generic.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-gen.zip` < temp.txt

expandlines $SRC/distrib/msw/makefile.rsp temp.txt
zip -@ -u `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-gen.zip` < temp.txt

expandlines $SRC/distrib/msw/msw.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-msw.zip` < temp.txt

expandlines $SRC/distrib/msw/makefile.rsp temp.txt
zip -@ -u `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-msw.zip` < temp.txt

expandlines $SRC/distrib/msw/gtk.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-gtk.zip` < temp.txt

expandlines $SRC/distrib/msw/makefile.rsp temp.txt
zip -@ -u `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-gtk.zip` < temp.txt

expandlines $SRC/distrib/msw/stubs.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-stubs.zip` < temp.txt

expandlines $SRC/distrib/msw/motif.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-mot.zip` < temp.txt

expandlines $SRC/distrib/msw/makefile.rsp temp.txt
zip -@ -u `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-mot.zip` < temp.txt


expandlines $SRC/distrib/msw/docsrc.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-DocSource.zip` < temp.txt

expandlines $SRC/distrib/msw/wx_hlp.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-WinHelp.zip` < temp.txt

expandlines $SRC/distrib/msw/wx_html.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-HTML.zip` < temp.txt

expandlines $SRC/distrib/msw/wx_pdf.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-PDF.zip` < temp.txt

expandlines $SRC/distrib/msw/wx_word.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-Word.zip` < temp.txt

expandlines $SRC/distrib/msw/wx_htb.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-HTB.zip` < temp.txt

expandlines $SRC/distrib/msw/wx_chm.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-HTMLHelp.zip` < temp.txt

# PDF/HTML docs that should go into the Windows setup because
# there are no WinHelp equivalents
expandlines $SRC/distrib/msw/extradoc.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/extradoc.zip` < temp.txt

# VC++ project files
expandlines $SRC/distrib/msw/vc.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-vc.zip` < temp.txt

# BC++ project files
expandlines $SRC/distrib/msw/bc.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-bc.zip` < temp.txt

# CodeWarrior project files
expandlines $SRC/distrib/msw/cw.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/wxWindows-$VERSION-cw.zip` < temp.txt

# OGL 3
expandlines $SRC/distrib/msw/ogl.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/ogl3.zip` < temp.txt

# MMedia
expandlines $SRC/distrib/msw/mmedia.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/mmedia.zip` < temp.txt

# STC (Scintilla widget)
expandlines $SRC/distrib/msw/stc.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/stc.zip` < temp.txt

# Tex2RTF
expandlines $SRC/distrib/msw/tex2rtf.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/tex2rtf2.zip` < temp.txt

# JPEG source
expandlines $SRC/distrib/msw/jpeg.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/jpeg.zip` < temp.txt

# TIFF source
expandlines $SRC/distrib/msw/tiff.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/tiff.zip` < temp.txt

# Dialog Editor source and binary
rm -f  $DEST/dialoged_source.zip
expandlines $SRC/distrib/msw/dialoged.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/dialoged_source.zip` < temp.txt
zip -j `$CYGPATHPROG -w $DEST/dialoged.zip` $DEST/dialoged_source.zip $SRC/bin/dialoged.exe $SRC/docs/winhelp/dialoged.hlp $SRC/docs/winhelp/dialoged.cnt
rm -f  $DEST/dialoged_source.zip

# Misc. utils not in the main distribution
expandlines $SRC/distrib/msw/utils.rsp temp.txt
zip -@ `$CYGPATHPROG -w $DEST/utils.zip` < temp.txt
expandlines $SRC/distrib/msw/utilmake.rsp temp.txt
zip -@ -u `$CYGPATHPROG -w $DEST/utilmake.zip` < temp.txt

rm -f temp.txt

# Make dialoged-win32.zip and tex2rtf-win32.zip

cd $SRC/bin

zip `$CYGPATHPROG -w $DEST/dialoged-win32.zip` dialoged.*
zip `$CYGPATHPROG -w $DEST/tex2rtf-win32.zip` tex2rtf.*

cp $SRC/docs/changes.txt $DEST
cp $SRC/docs/msw/install.txt $DEST/install_msw.txt
cp $SRC/docs/motif/install.txt $DEST/install_motif.txt
cp $SRC/docs/gtk/install.txt $DEST/install_gtk.txt
cp $SRC/docs/readme.txt $DEST
cp $SRC/docs/motif/readme.txt $DEST/readme_motif.txt
cp $SRC/docs/gtk/readme.txt $DEST/readme_gtk.txt
cp $SRC/docs/msw/readme.txt $DEST/readme_msw.txt
cp $SRC/docs/readme_vms.txt $DEST
cp $SRC/docs/motif/makewxmotif $DEST
cp $SRC/docs/gtk/makewxgtk $DEST

# Skip WISE setup if WISE is 0.
if [ "$WISE" = "1" ]; then
    dowise
fi

echo wxWindows archived.


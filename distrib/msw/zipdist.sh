#! /bin/sh

# Zip up an external, generic + Windows distribution of wxWindows 2
src=`cygpath -u $WXWIN`
dest=$src/deliver
wise=0
wiseonly=0

# Set this to the required version
version=2.3.0

dowise()
{
    # Make dialoged-win32.zip and tex2rtf-win32.zip

    cd $src/bin

    zip $dest/dialoged-win32.zip dialoged.*
    zip $dest/tex2rtf-win32.zip tex2rtf.*

    cd $dest

    # Unzip the Windows files into 'wx'
    mkdir $dest/wx

    # After this change of directory, we're in the
    # temporary 'wx' directory and not acting on
    # the source wxWindows directory.
    cd $dest/wx
    unzip -o ../wxWindows-$version-msw.zip
    unzip -o ../wxWindows-$version-gen.zip
    unzip -o ../wxWindows-$version-vc.zip
    unzip -o ../wxWindows-$version-bc.zip
    unzip -o ../wxWindows-$version-HTMLHelp.zip
    unzip -o ../extradoc.zip
    # Need Word file, for Remstar DB classes
    unzip -o ../wxWindows-$version-Word.zip
    unzip -o ../ogl3.zip
    unzip -o ../jpeg.zip
    unzip -o ../tiff.zip
    unzip -o ../tex2rtf2.zip

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
    rmdir /S contrib/docs/latex/ogl
    rm -f src/mingegcs.bat
    rm -f distrib

    # Now copy some binary files to 'bin'
    mkdir bin
    cp $src/bin/dialoged.exe bin
    cp $src/bin/tex2rtf.exe bin
    cp $src/bin/dbgview.* bin
    cp $src/bin/life.exe bin
    cp $src/docs/winhelp/dialoged.hlp $src/docs/winhelp/dialoged.cnt bin
    cp $src/docs/winhelp/tex2rtf.hlp $src/docs/winhelp/tex2rtf.cnt bin

    # Make wxMSW-xxx.zip
    zip -r ../wxMSW-$version.zip *

    # Time to regenerate the WISE install script, wxwin2.wse.
    # NB: if you've changed wxwin2.wse using WISE, call splitwise.exe
    # from within distrib/msw, to split off wisetop.txt and wisebott.txt.
    echo Calling 'makewise' to generate wxwin2.wse...

    # TODO!!!!
    call $WXWIN/distrib/msw/makewise.bat

    rm -f $dest/setup.*

    # Now invoke WISE install on the new wxwin2.wse
    echo Invoking WISE...
    /c/progra~1/wise/wise32.exe /C $WXWIN\distrib\msw\wxwin2.wse
    echo Press return to continue with the wxWindows distribution...
    read dummy

    cd $dest

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

    # Put all the setup files into a single zip archive.
    zip wxMSW-$version-setup.zip readme.txt setup.*
}


# Process command line options.
for i in "$@"; do
    case "$i" in
	--wise) wise=1 ;;
	--wiseonly) wiseonly=1 ;;
	*)
	    echo Usage: $0 "[ options ]" 1>&2
	    echo Options:
	    echo "    --help         Display this help message"
	    echo "    --wise         Build setup.exe"
	    exit 1
	    ;;
    esac
done

if [ ! -d "$src" ]; then
    echo $src not found.
    exit 1
fi

if [ ! -d "$dest" ]; then
    echo $dest not found.
    exit 1
fi

echo Creating distribution in $dest. Press return to continue.
read dummy

if [ "$wiseonly" != "0" ]; then
        dowise
        exit 0
fi

# Remove all existing files
rm -f  $dest/wx*.zip
rm -f  $dest/*.htb
rm -f  $dest/ogl3.zip
rm -f  $dest/tex2rtf2.zip
rm -f  $dest/jpeg.zip
rm -f  $dest/tiff.zip
rm -f  $dest/dialoged.zip
rm -f  $dest/utils.zip
rm -f  $dest/extradoc.zip
rm -f  $dest/*-win32.zip

if [ ! -d "$dest/wx" ]; then
    rm -f -r $dest/wx
fi


cd $src
echo Zipping...

# Below is the old-style separated-out format. This is retained only
# for local use, and for creating wxMSW-xxx.zip.
ls `cat $src/distrib/msw/generic.rsp` | zip -@ $dest/wxWindows-$version-gen.zip
ls `cat $src/distrib/msw/makefile.rsp` | zip -@ -u $dest/wxWindows-$version-gen.zip
ls `cat $src/distrib/msw/msw.rsp` | zip -@ $dest/wxWindows-$version-msw.zip
ls `cat $src/distrib/msw/makefile.rsp` | zip -@ -u $dest/wxWindows-$version-msw.zip
ls `cat $src/distrib/msw/gtk.rsp` | zip -@ $dest/wxWindows-$version-gtk.zip
ls `cat $src/distrib/msw/makefile.rsp` | zip -@ -u $dest/wxWindows-$version-gtk.zip
ls `cat $src/distrib/msw/stubs.rsp` | zip -@ $dest/wxWindows-$version-stubs.zip
ls `cat $src/distrib/msw/motif.rsp` | zip -@ $dest/wxWindows-$version-mot.zip
ls `cat $src/distrib/msw/makefile.rsp` | zip -@ -u $dest/wxWindows-$version-mot.zip
# ls `cat $src/distrib/msw/user.rsp` | zip -@ $dest/wxWindows-$version-user.zip # Obsolete

ls `cat $src/distrib/msw/docsrc.rsp` | zip -@ $dest/wxWindows-$version-DocSource.zip
ls `cat $src/distrib/msw/wx_hlp.rsp` | zip -@ $dest/wxWindows-$version-WinHelp.zip
ls `cat $src/distrib/msw/wx_html.rsp` | zip -@ $dest/wxWindows-$version-HTML.zip
ls `cat $src/distrib/msw/wx_pdf.rsp` | zip -@ $dest/wxWindows-$version-PDF.zip
ls `cat $src/distrib/msw/wx_word.rsp` | zip -@ $dest/wxWindows-$version-Word.zip
ls `cat $src/distrib/msw/wx_htb.rsp` | zip -@ $dest/wxWindows-$version-HTB.zip
ls `cat $src/distrib/msw/wx_chm.rsp` | zip -@ $dest/wxWindows-$version-HTMLHelp.zip

# PDF/HTML docs that should go into the Windows setup because
# there are no WinHelp equivalents
ls `cat $src/distrib/msw/extradoc.rsp` | zip -@ $dest/extradoc.zip

# VC++ project files
ls `cat $src/distrib/msw/vc.rsp` | zip -@ $dest/wxWindows-$version-vc.zip

# BC++ project files
ls `cat $src/distrib/msw/bc.rsp` | zip -@ $dest/wxWindows-$version-bc.zip

# CodeWarrior project files
ls `cat $src/distrib/msw/cw.rsp` | zip -@ $dest/wxWindows-$version-cw.zip

# OGL 3
ls `cat $src/distrib/msw/ogl.rsp` | zip -@ $dest/ogl3.zip

# MMedia
ls `cat $src/distrib/msw/mmedia.rsp` | zip -@ $dest/mmedia.zip

# STC (Scintilla widget)
ls `cat $src/distrib/msw/stc.rsp` | zip -@ $dest/stc.zip

# GLCanvas: obsolete, now in main library
# ls `cat $src/distrib/msw/glcanvas.rsp` | zip -@ $dest/glcanvas.zip

# Tex2RTF
ls `cat $src/distrib/msw/tex2rtf.rsp` | zip -@ $dest/tex2rtf2.zip

# JPEG source
ls `cat $src/distrib/msw/jpeg.rsp` | zip -@ $dest/jpeg.zip

# TIFF source
ls `cat $src/distrib/msw/tiff.rsp` | zip -@ $dest/tiff.zip

# Dialog Editor source and binary
rm -f  $dest/dialoged_source.zip
ls `cat $src/distrib/msw/dialoged.rsp` | zip -@ $dest/dialoged_source.zip
zip -j $dest/dialoged.zip $dest/dialoged_source.zip $src/bin/dialoged.exe $src/docs/winhelp/dialoged.hlp $src/docs/winhelp/dialoged.cnt
rm -f  $dest/dialoged_source.zip

# Misc. utils not in the main distribution
ls `cat $src/distrib/msw/utils.rsp` | zip -@ $dest/utils.zip
ls `cat $src/distrib/msw/utilmake.rsp` | zip -@ -u $dest/utils.zip

cp $src/docs/changes.txt $dest
cp $src/docs/msw/install.txt $dest/install_msw.txt
cp $src/docs/motif/install.txt $dest/install_motif.txt
cp $src/docs/gtk/install.txt $dest/install_gtk.txt
cp $src/docs/readme.txt $dest
cp $src/docs/motif/readme.txt $dest/readme_motif.txt
cp $src/docs/gtk/readme.txt $dest/readme_gtk.txt
cp $src/docs/msw/readme.txt $dest/readme_msw.txt
cp $src/docs/readme_vms.txt $dest
cp $src/docs/motif/makewxmotif $dest
cp $src/docs/gtk/makewxgtk $dest

# Skip WISE setup if wise is 0.
if [ "$wise" = "1"] ; then
    dowise
fi

echo wxWindows archived.


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
    rm -f -r contrib/docs/latex/ogl
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

    sh $WXWIN/distrib/msw/makewise.sh

    rm -f $dest/setup.*

    # Now invoke WISE install on the new wxwin2.wse
    echo Invoking WISE...
    /c/progra~1/wise/wise32.exe /C $WXWIN\\distrib\\msw\\wxwin2.wse
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

    mv setup.w09 s
    mv s setup.w09

    # Put all the setup files into a single zip archive.
    zip wxMSW-$version-setup.zip readme.txt setup.*
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

# We can't use e.g. this:
# ls `cat $src/distrib/msw/makefile.rsp` zip -@ -u $dest/wxWindows-$version-gen.zip
# because there's not enough space on the command line, plus we need to ignore the
# blank lines.

expandlines $src/distrib/msw/generic.rsp temp.txt
zip -@ $dest/wxWindows-$version-gen.zip < temp.txt

expandlines $src/distrib/msw/makefile.rsp temp.txt
zip -@ -u $dest/wxWindows-$version-gen.zip < temp.txt

expandlines $src/distrib/msw/msw.rsp temp.txt
zip -@ $dest/wxWindows-$version-msw.zip < temp.txt

expandlines $src/distrib/msw/makefile.rsp temp.txt
zip -@ -u $dest/wxWindows-$version-msw.zip < temp.txt

expandlines $src/distrib/msw/gtk.rsp temp.txt
zip -@ $dest/wxWindows-$version-gtk.zip < temp.txt

expandlines $src/distrib/msw/makefile.rsp temp.txt
zip -@ -u $dest/wxWindows-$version-gtk.zip < temp.txt

expandlines $src/distrib/msw/stubs.rsp temp.txt
zip -@ $dest/wxWindows-$version-stubs.zip < temp.txt

expandlines $src/distrib/msw/motif.rsp temp.txt
zip -@ $dest/wxWindows-$version-mot.zip < temp.txt

expandlines $src/distrib/msw/makefile.rsp temp.txt
zip -@ -u $dest/wxWindows-$version-mot.zip < temp.txt


expandlines $src/distrib/msw/docsrc.rsp temp.txt
zip -@ $dest/wxWindows-$version-DocSource.zip < temp.txt

expandlines $src/distrib/msw/wx_hlp.rsp temp.txt
zip -@ $dest/wxWindows-$version-WinHelp.zip < temp.txt

expandlines $src/distrib/msw/wx_html.rsp temp.txt
zip -@ $dest/wxWindows-$version-HTML.zip < temp.txt

expandlines $src/distrib/msw/wx_pdf.rsp temp.txt
zip -@ $dest/wxWindows-$version-PDF.zip < temp.txt

expandlines $src/distrib/msw/wx_word.rsp temp.txt
zip -@ $dest/wxWindows-$version-Word.zip < temp.txt

expandlines $src/distrib/msw/wx_htb.rsp temp.txt
zip -@ $dest/wxWindows-$version-HTB.zip < temp.txt

expandlines $src/distrib/msw/wx_chm.rsp temp.txt
zip -@ $dest/wxWindows-$version-HTMLHelp.zip < temp.txt

# PDF/HTML docs that should go into the Windows setup because
# there are no WinHelp equivalents
expandlines $src/distrib/msw/extradoc.rsp temp.txt
zip -@ $dest/extradoc.zip < temp.txt

# VC++ project files
expandlines $src/distrib/msw/vc.rsp temp.txt
zip -@ $dest/wxWindows-$version-vc.zip < temp.txt

# BC++ project files
expandlines $src/distrib/msw/bc.rsp temp.txt
zip -@ $dest/wxWindows-$version-bc.zip < temp.txt

# CodeWarrior project files
expandlines $src/distrib/msw/cw.rsp temp.txt
zip -@ $dest/wxWindows-$version-cw.zip < temp.txt

# OGL 3
expandlines $src/distrib/msw/ogl.rsp temp.txt
zip -@ $dest/ogl3.zip < temp.txt

# MMedia
expandlines $src/distrib/msw/mmedia.rsp temp.txt
zip -@ $dest/mmedia.zip < temp.txt

# STC (Scintilla widget)
expandlines $src/distrib/msw/stc.rsp temp.txt
zip -@ $dest/stc.zip < temp.txt

# Tex2RTF
expandlines $src/distrib/msw/tex2rtf.rsp temp.txt
zip -@ $dest/tex2rtf2.zip < temp.txt

# JPEG source
expandlines $src/distrib/msw/jpeg.rsp temp.txt
zip -@ $dest/jpeg.zip < temp.txt

# TIFF source
expandlines $src/distrib/msw/tiff.rsp temp.txt
zip -@ $dest/tiff.zip < temp.txt

# Dialog Editor source and binary
rm -f  $dest/dialoged_source.zip
expandlines $src/distrib/msw/dialoged.rsp temp.txt
zip -@ $dest/dialoged_source.zip < temp.txt
zip -j $dest/dialoged.zip $dest/dialoged_source.zip $src/bin/dialoged.exe $src/docs/winhelp/dialoged.hlp $src/docs/winhelp/dialoged.cnt
rm -f  $dest/dialoged_source.zip

# Misc. utils not in the main distribution
expandlines $src/distrib/msw/utils.rsp temp.txt
zip -@ $dest/utils.zip < temp.txt
expandlines $src/distrib/msw/utilmake.rsp temp.txt
zip -@ -u $dest/utilmake.zip < temp.txt

rm -f temp.txt

# Make dialoged-win32.zip and tex2rtf-win32.zip

cd $src/bin

zip $dest/dialoged-win32.zip dialoged.*
zip $dest/tex2rtf-win32.zip tex2rtf.*

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
if [ "$wise" = "1" ]; then
    dowise
fi

echo wxWindows archived.


#!/bin/sh

# Make a distribution of an application on MSW.
# Example:
# ../distrib/msw/makesetup.sh --wxmsw --verbose &> log

# If your zip accepts Cygwin-style paths, then
# use cygpath, else substitute echo
CYGPATHPROG=cygpath
#CYGPATHPROG=echo

INNO=1
SPINMSW=0
SPINMAC=0
SPINOS2=0
SPINDOCS=0
SPINALL=0
SPINWXALL=0
SPINBASE=0
GETMAKEFILES=0
VERBOSE=0
ZIPFLAGS=

PROGNAME=$0
SCRIPTDIR=$WXWIN/distrib/msw
WEBFILES=c:/wx2dev/wxWebSite
# Set this to the required version
VERSION=2.6.2

. $SCRIPTDIR/setup.var

doreplace()
{
    thefile=$1
    theexpr=$2

    if [ -f $thefile ]; then
      sed -e "$theexpr" < $thefile > $thefile.tmp
      mv $thefile.tmp $thefile
    else
      echo "*** $thefile not found."
    fi
}

unix2dosname()
{
    echo $1 | sed -e "s/\//\\\\\\\/g" > /tmp/filename.tmp
    RETVALUE=`cat /tmp/filename.tmp`
    rm -f /tmp/filename.tmp
}

unix2dosname2()
{
    echo $1 | sed -e "s/\//\\\\/g" > /tmp/filename.tmp
    RETVALUE=`cat /tmp/filename.tmp`
    rm -f /tmp/filename.tmp
}

doinit()
{
    if [ "$VERBOSE" != "1" ]; then
        ZIPFLAGS=-q
    fi
}

rearchive()
{
    archive=$1
    dirname=$2
    changeto=$3

    echo Re-archiving $archive as $dirname

    pushd $changeto

    if [ -d $dirname ]; then
        rm -f -r $dirname
    fi
    mkdir $dirname
    cd $dirname
    unzip $ZIPFLAGS ../$archive
    cd ..
    rm -f $archive
    zip $ZIPFLAGS -r $archive $dirname/*

    popd
}

rearchivetar()
{
    archive=$1
    dirname=$2
    changeto=$3

    echo Re-tarring $archive as $dirname

    pushd $changeto

    if [ -d $dirname ]; then
        rm -f -r $dirname
    fi
    mkdir $dirname
    cd $dirname
    tar xfz ../$archive
    cd ..
    rm -f $archive
    tar cfz $archive $dirname/*

    popd
}

# Find the version from wx/version.h
# Not yet used
findversion()
{
	echo "#include <stdio.h>" > /tmp/appver.c
	echo "#include \"$VERSIONSYMBOLFILE\"" >> /tmp/appver.c
	echo "int main() { printf(\"%.2f\", $VERSIONSYMBOL); }" >> /tmp/appver.c
	gcc /tmp/appver.c -I$APPDIR -o /tmp/appver
    VERSION=`/tmp/appver`
	rm -f /tmp/appver /tmp/appver.c
}

dospinos2()
{
    cd $APPDIR
    echo Zipping OS/2...

    # Zip up the complete wxOS2-xxx.zip file
    zip $ZIPFLAGS -@ $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/generic.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/os2.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxOS2-$VERSION.zip  < $APPDIR/distrib/msw/jpeg.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/tiff.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/tiff.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/jpeg.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/utils.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/tex2rtf.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/ogl.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/xml.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/contrib.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/deprecated.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxOS2-$VERSION.zip < $APPDIR/distrib/msw/makefile.rsp

    # Rearchive under wxWidgets-$VERSION
    if [ -d $DESTDIR/wxWidgets-$VERSION ]; then
        rm -f -r $DESTDIR/wxWidgets-$VERSION
    fi

    mkdir $DESTDIR/wxWidgets-$VERSION
    cd $DESTDIR/wxWidgets-$VERSION
    unzip $ZIPFLAGS ../wxOS2-$VERSION.zip
    # No longer do this, configure should be OK
    # echo Overwriting with OS2-specific versions of configure files...
    # unzip $ZIPFLAGS -o $APPDIR/distrib/os2/os2-specific.zip
    rm -f src/gtk/descrip.mms src/motif/descrip.mms docs/pdf/*.pdf
    rm -f src/tiff/*.mcp src/jpeg/*.mcp src/png/*.mcp src/zlib/*.mcp
    rm -f -r docs/html/tex2rtf

    # echo Making OS/2 files lower case...
    # no longer necessary
    # $SCRIPTDIR/namedown include/wx/os2/*.H
    # $SCRIPTDIR/namedown src/os2/*.CPP src/os2/*.I

    echo Copying readme files...
    cp $APPDIR/docs/os2/install.txt INSTALL-OS2.txt
    cp $APPDIR/docs/licence.txt LICENCE.txt
    cp $APPDIR/docs/lgpl.txt COPYING.LIB
    cp $APPDIR/docs/changes.txt CHANGES.txt
    cp $APPDIR/docs/readme.txt README.txt

    cd $DESTDIR

    rm -f wxOS2-$VERSION.zip
    zip $ZIPFLAGS -r wxOS2-$VERSION.zip wxWidgets-$VERSION/*
}

dospinmac()
{
    echo Spinning wxMac...
    cd $APPDIR

    echo Zipping wxMac distribution

    cp $APPDIR/include/wx/mac/setup0.h $APPDIR/include/wx/setup.h

    zip $ZIPFLAGS -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/generic.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/mac.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/cocoa.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/cw_mac.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/tiff.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/jpeg.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/utils.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/tex2rtf.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/ogl.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/xml.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/stc.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/contrib.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/deprecated.rsp
    zip $ZIPFLAGS -u -@ $DESTDIR/wxMac-$VERSION.zip < $APPDIR/distrib/msw/makefile.rsp

    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-CW-Mac.zip < $APPDIR/distrib/msw/cw_mac.rsp

    rm -f $APPDIR/include/wx/setup.h

    if [ -d $DESTDIR/wxWidgets-$VERSION ]; then
        rm -f -r $DESTDIR/wxWidgets-$VERSION
    fi

    mkdir $DESTDIR/wxWidgets-$VERSION
    cd $DESTDIR/wxWidgets-$VERSION
    unzip $ZIPFLAGS ../wxMac-$VERSION.zip
    rm -f src/gtk/descrip.mms src/motif/descrip.mms docs/pdf/*.pdf
    rm -f -r docs/html/tex2rtf docs/htmlhelp

    echo Copying readme files...
    cp $APPDIR/docs/mac/readme.txt README-MAC.txt
    cp $APPDIR/docs/mac/install.txt INSTALL-MAC.txt
    cp $APPDIR/docs/licence.txt LICENCE.txt
    cp $APPDIR/docs/lgpl.txt COPYING.LIB
    cp $APPDIR/docs/changes.txt CHANGES.txt
    cp $APPDIR/docs/readme.txt README.txt

    cd $DESTDIR

    rm -f wxMac-$VERSION.zip
    zip $ZIPFLAGS -r wxMac-$VERSION.zip wxWidgets-$VERSION/*
}

dospinmsw()
{
    echo Zipping wxMSW...
    cd $APPDIR

    # Create wxWidgets-$VERSION-win.zip which is used to create wxMSW
    echo Zipping individual components
    rm -f $DESTDIR/wxWidgets-$VERSION-win.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/generic.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/makefile.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/msw.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/ogl.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/mmedia.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/stc.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/tex2rtf.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/jpeg.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/tiff.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/xml.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/contrib.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/deprecated.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/utils.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/utilmake.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/univ.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/wince.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-win.zip < $APPDIR/distrib/msw/palmos.rsp
}

dospinwxall()
{
    echo Zipping wxAll...
    cd $APPDIR

    echo Zipping individual components
    rm -f $DESTDIR/wxWidgets-$VERSION-all.zip

    # Save adding all the wxMSW files again    
    if [ ! -f $DESTDIR/wxWidgets-$VERSION-win.zip ]; then
        dospinmsw
    fi
    cp $DESTDIR/wxWidgets-$VERSION-win.zip $DESTDIR/wxWidgets-$VERSION-all.zip
    
    cat $APPDIR/distrib/msw/cw_mac.rsp $APPDIR/distrib/msw/vc.rsp $APPDIR/distrib/msw/x11.rsp $APPDIR/distrib/msw/gtk.rsp $APPDIR/distrib/msw/cocoa.rsp $APPDIR/distrib/msw/motif.rsp $APPDIR/distrib/msw/mac.rsp $APPDIR/distrib/msw/mgl.rsp $APPDIR/distrib/msw/os2.rsp $APPDIR/distrib/msw/palmos.rsp | sort | uniq > /tmp/all.txt
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-all.zip < /tmp/all.txt

    if [ -d $DESTDIR/wxWidgets-$VERSION ]; then
        rm -f -r $DESTDIR/wxWidgets-$VERSION
    fi

    mkdir $DESTDIR/wxWidgets-$VERSION
    cd $DESTDIR/wxWidgets-$VERSION
    unzip $ZIPFLAGS ../wxWidgets-$VERSION-all.zip

    echo Copying readme files...
    cp $APPDIR/docs/licence.txt LICENCE.txt
    cp $APPDIR/docs/lgpl.txt COPYING.LIB
    cp $APPDIR/docs/changes.txt CHANGES.txt
    cp $APPDIR/docs/readme.txt README.txt

    cd $DESTDIR

    rm -f $DESTDIR/wxWidgets-$VERSION-all.zip
    zip $ZIPFLAGS -r wxWidgets-$VERSION.zip wxWidgets-$VERSION/*
}

dospinbase()
{
    cd $APPDIR

    echo Zipping wxBase...
    rm -f $DESTDIR/wxBase-$VERSION.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxBase-$VERSION.zip < $APPDIR/distrib/msw/base.rsp

    if [ -d $DESTDIR/wxWidgets-$VERSION ]; then
        rm -f -r $DESTDIR/wxWidgets-$VERSION
    fi

    mkdir $DESTDIR/wxWidgets-$VERSION
    cd $DESTDIR/wxWidgets-$VERSION
    unzip $ZIPFLAGS ../wxBase-$VERSION.zip

    echo Copying readme files...
    cp $APPDIR/docs/base/readme.txt README.txt
    cp $APPDIR/docs/licence.txt LICENCE.txt
    cp $APPDIR/docs/lgpl.txt COPYING.LIB
    cp $APPDIR/docs/changes.txt CHANGES.txt
    cp $APPDIR/docs/readme.txt README2.txt

    cd $DESTDIR

    rm -f wxBase-$VERSION.zip
    zip $ZIPFLAGS -r wxBase-$VERSION.zip wxWidgets-$VERSION/*
}

dospindocs()
{
    cd $APPDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-DocSource.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-DocSource.zip < $APPDIR/distrib/msw/docsrc.rsp
    rearchive wxWidgets-$VERSION-DocSource.zip wxWidgets-$VERSION $DESTDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-WinHelp.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-WinHelp.zip < $APPDIR/distrib/msw/wx_hlp.rsp
    rearchive wxWidgets-$VERSION-WinHelp.zip wxWidgets-$VERSION $DESTDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-HTML.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-HTML.zip < $APPDIR/distrib/msw/wx_html.rsp
    rearchive wxWidgets-$VERSION-HTML.zip wxWidgets-$VERSION $DESTDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-PDF.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-PDF.zip < $APPDIR/distrib/msw/wx_pdf.rsp
    rearchive wxWidgets-$VERSION-PDF.zip wxWidgets-$VERSION $DESTDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-PDF.tar.gz
    tar zcf /c/wx2dev/wxWindows/deliver/wxWidgets-$VERSION-PDF.tar.gz docs/pdf/*.pdf
    rearchivetar wxWidgets-$VERSION-PDF.tar.gz wxWidgets-$VERSION /c/wx2dev/wxWindows/deliver

    # zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-Word.zip < $APPDIR/distrib/msw/wx_word.rsp
    # rearchive wxWidgets-$VERSION-Word.zip wxWidgets-$VERSION $DESTDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-HTB.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-HTB.zip < $APPDIR/distrib/msw/wx_htb.rsp
    rearchive wxWidgets-$VERSION-HTB.zip wxWidgets-$VERSION $DESTDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-HTMLHelp.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-HTMLHelp.zip < $APPDIR/distrib/msw/wx_chm.rsp
    rearchive wxWidgets-$VERSION-HTMLHelp.zip wxWidgets-$VERSION $DESTDIR

    # Add Linuxy docs to a separate archive to be transported to Linux for the
    # Linux-based releases
    echo Creating $DESTDIR/wxWidgets-$VERSION-LinuxDocs.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-LinuxDocs.zip < $APPDIR/distrib/msw/wx_html.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-LinuxDocs.zip < $APPDIR/distrib/msw/wx_pdf.rsp
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-LinuxDocs.zip < $APPDIR/distrib/msw/wx_htb.rsp

    # PDF/HTML docs that should go into the Windows setup because
    # there are no WinHelp equivalents
    echo Creating $DESTDIR/wxWidgets-$VERSION-ExtraDoc.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-ExtraDoc.zip < $APPDIR/distrib/msw/extradoc.rsp
    rearchive wxWidgets-$VERSION-ExtraDoc.zip wxWidgets-$VERSION $DESTDIR
}

dospinmisc()
{
    cd $APPDIR

    # zip up Univ-specific files
    echo Creating $DESTDIR/wxWidgets-$VERSION-Univ.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-Univ.zip < $APPDIR/distrib/msw/univ.rsp
    rearchive wxWidgets-$VERSION-Univ.zip wxWidgets-$VERSION $DESTDIR

    # VC++ project files
    echo Creating $DESTDIR/wxWidgets-$VERSION-VC.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-VC.zip < $APPDIR/distrib/msw/vc.rsp
    # rearchive wxWidgets-$VERSION-VC.zip wxWidgets-$VERSION $DESTDIR

    # eVC++ project files
    echo Creating $DESTDIR/wxWidgets-$VERSION-eVC.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-eVC.zip < $APPDIR/distrib/msw/wince.rsp
    # rearchive wxWidgets-$VERSION-eVC.zip wxWidgets-$VERSION $DESTDIR

    # DMC project files
    echo Creating $DESTDIR/wxWidgets-$VERSION-DMC.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-DMC.zip < $APPDIR/distrib/msw/dmc.rsp
    # rearchive wxWidgets-$VERSION-DMC.zip wxWidgets-$VERSION $DESTDIR

    # BC++ project files
    echo Creating $DESTDIR/wxWidgets-$VERSION-BC.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-BC.zip < $APPDIR/distrib/msw/bc.rsp
    # rearchive wxWidgets-$VERSION-BC.zip wxWidgets-$VERSION $DESTDIR

    # CodeWarrior project files
    echo Creating $DESTDIR/wxWidgets-$VERSION-CW.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-CW.zip < $APPDIR/distrib/msw/cw.rsp
    # rearchive wxWidgets-$VERSION-CW.zip wxWidgets-$VERSION $DESTDIR
}

dospinsetup()
{
    cd $DESTDIR

    # Put all archives for transit to Linux in a zip file
    echo Creating $DESTDIR/wxWidgets-$VERSION-LinuxTransit.zip
    rm -f $DESTDIR/wxWidgets-$VERSION-LinuxTransit.zip
    zip $ZIPFLAGS $DESTDIR/wxWidgets-$VERSION-LinuxTransit.zip wxWidgets-$VERSION-LinuxDocs.zip wxWidgets-$VERSION-VC.zip wxWidgets-$VERSION-DMC.zip wxWidgets-$VERSION-eVC.zip wxWidgets-$VERSION-CW-Mac.zip

    if [ ! -f $DESTDIR/wxWidgets-$VERSION-HTMLHelp.zip ]; then
        cd $APPDIR
        echo Creating $DESTDIR/wxWidgets-$VERSION-HTMLHelp.zip
        zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-HTMLHelp.zip < $APPDIR/distrib/msw/wx_chm.rsp
        rearchive wxWidgets-$VERSION-HTMLHelp.zip wxWidgets-$VERSION $DESTDIR
        cd $DESTDIR
    fi

    if [ ! -f $DESTDIR/wxWidgets-$VERSION-ExtraDoc.zip ]; then
        cd $APPDIR
        echo Creating $DESTDIR/wxWidgets-$VERSION-ExtraDoc.zip
        zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-ExtraDoc.zip < $APPDIR/distrib/msw/extradoc.rsp
        rearchive wxWidgets-$VERSION-ExtraDoc.zip wxWidgets-$VERSION $DESTDIR
        cd $DESTDIR
    fi

    rm -f -r wxWidgets-$VERSION

    echo Unzipping the Windows files into wxWidgets-$VERSION

    mkdir -p wxWidgets-$VERSION

    unzip $ZIPFLAGS -o wxWidgets-$VERSION-win.zip -d wxWidgets-$VERSION
    unzip $ZIPFLAGS -o wxWidgets-$VERSION-VC.zip -d wxWidgets-$VERSION
    unzip $ZIPFLAGS -o wxWidgets-$VERSION-DMC.zip -d wxWidgets-$VERSION
    unzip $ZIPFLAGS -o wxWidgets-$VERSION-BC.zip -d wxWidgets-$VERSION
    unzip $ZIPFLAGS -o wxWidgets-$VERSION-CW.zip -d wxWidgets-$VERSION

    unzip $ZIPFLAGS -o wxWidgets-$VERSION-HTMLHelp.zip
    unzip $ZIPFLAGS -o wxWidgets-$VERSION-ExtraDoc.zip

    # After this change of directory, we're in the
    # temporary 'wx' directory and not acting on
    # the source wxWidgets directory.
    cd $DESTDIR/wxWidgets-$VERSION

    # Now delete a few files that are unnecessary
    #attrib -R *
    rm -f BuildCVS.txt descrip.mms
    rm -f setup.h_vms
    rm -f docs/html/wxbook.htm docs/html/roadmap.htm
    rm -f -r contrib/docs/latex/ogl
    rm -f src/mingegcs.bat
    rm -f -r distrib
    rm -f *.spec
    rm -f src/gtk/descrip.mms src/motif/descrip.mms

    echo Copying readme files...
    cp $APPDIR/docs/msw/readme.txt README-MSW.txt
    cp $APPDIR/docs/msw/install.txt INSTALL-MSW.txt
    cp $APPDIR/docs/licence.txt LICENCE.txt
    cp $APPDIR/docs/lgpl.txt COPYING.LIB
    cp $APPDIR/docs/changes.txt CHANGES.txt
    cp $APPDIR/docs/readme.txt README.txt

    # Now cp some binary files to 'bin'
    if [ ! -d bin ]; then
        mkdir bin
    fi
    cp $APPDIR/bin/tex2rtf.exe bin
    cp $APPDIR/bin/tex2rtf.chm bin
    cp $APPDIR/bin/widgets.exe bin
    cp $APPDIR/bin/life.exe bin
    cp $APPDIR/demos/life/breeder.lif bin
    cp $APPDIR/docs/htmlhelp/tex2rtf.chm bin

    if [ ! -d docs/pdf ]; then
        mkdir docs/pdf
    fi
    #cp $APPDIR/docs/pdf/wxTutorial.pdf docs/pdf

    # Make wxMSW-xxx.zip
    cd $DESTDIR
    zip $ZIPFLAGS -r wxMSW-$VERSION.zip wxWidgets-$VERSION/*
    cd wxWidgets-$VERSION

    echo Generating $SETUPSCRIPTNAME
    rm -f $SETUPSCRIPTNAME

    sh $SCRIPTDIR/makeinno.sh $SETUPIMAGEDIR $INNOTOP $INNOBOTTOM $SETUPSCRIPTNAME

    if [ ! -f $SETUPSCRIPTNAME ]; then
        echo "*** Error - something went wrong with the script file generation."
        exit 1
    fi

    # Now replace %VERSION% with the real application version, and other
    # variables
    echo Replacing variables in the setup script
    doreplace $SETUPSCRIPTNAME "s/%VERSION%/$VERSION/g"
    doreplace $SETUPSCRIPTNAME "s/%COPYRIGHTHOLDER%/$AUTHOR/g"
    doreplace $SETUPSCRIPTNAME "s/%VENDOR%/$VENDOR/g"

    unix2dosname $READMEFILE
    doreplace $SETUPSCRIPTNAME "s;%READMEFILE%;$RETVALUE;g"

    unix2dosname $READMEAFTERFILE
    doreplace $SETUPSCRIPTNAME "s;%READMEAFTERFILE%;$RETVALUE;g"

    unix2dosname $LICENSEFILE
    doreplace $SETUPSCRIPTNAME "s;%LICENSEFILE%;$RETVALUE;g"

    doreplace $SETUPSCRIPTNAME "s/%APPNAME%/$APPNAME/g"
    doreplace $SETUPSCRIPTNAME "s/%APPTITLE%/$APPTITLE/g"

    unix2dosname $SETUPIMAGEDIR
    doreplace $SETUPSCRIPTNAME "s;%SOURCEDIR%;$RETVALUE;g"

    unix2dosname $DESTDIR
    doreplace $SETUPSCRIPTNAME "s;%OUTPUTDIR%;$RETVALUE;g"

    doreplace $SETUPSCRIPTNAME "s/%APPEXTENSION%/$APPEXTENSION/g"

    # FIXME: how do we get the first name in the list?
	if [ "$MANUALFILES" != "" ]; then
	    HELPFILE=`basename $MANUALFILES`
		unix2dosname $HELPFILE
		doreplace $SETUPSCRIPTNAME "s;%HELPFILE%;$RETVALUE;g"
    fi

    rm -f $DESTDIR/setup*.* $DESTDIR/wxMSW-$VERSION-Setup.exe

    # Inno Setup complains if this step is not done
    unix2dos --unix2dos $SETUPSCRIPTNAME
    
    # Now invoke INNO compiler on the new ISS file
    # First, make a DOS filename or Inno Setup will get confused.

    unix2dosname2 $SETUPSCRIPTNAME
    DOSFILENAME=$RETVALUE

    # Note: the double slash is Mingw32/MSYS convention for
    # denoting a switch, that must not be converted into
    # a path (otherwise /c = c:/)

    cd `dirname $SETUPSCRIPTNAME`
    BASESCRIPTNAME=`basename $SETUPSCRIPTNAME`
    echo Invoking Inno Setup compiler on $BASESCRIPTNAME

    "$SETUPCOMPILER" //cc $BASESCRIPTNAME

    if [ ! -f $DESTDIR/setup.exe ]; then
        echo "*** Error - the setup.exe was not generated."
        exit
    fi

    cd $DESTDIR
    mv setup.exe wxMSW-$VERSION-Setup.exe

    # echo Putting all the setup files into a single zip archive
    # zip wxMSW-$VERSION-setup.zip readme-$VERSION.txt setup*.*

    rm -f wxWidgets-$VERSION-win.zip
    rm -f wxWidgets-$VERSION-ExtraDoc.zip
    rm -f wxWidgets-$VERSION-DMC.zip
    rm -f wxWidgets-$VERSION-eVC.zip
    rm -f wxWidgets-$VERSION-Univ.zip
    rm -f wxWidgets-$VERSION-VC.zip
    rm -f wxWidgets-$VERSION-DocSource.zip
    rm -f wxWidgets-$VERSION-LinuxDocs.zip

    echo If you saw no warnings or errors, $APPTITLE was successfully spun.
    echo
}

makesetup()
{
#    if [ -d $SETUPIMAGEDIR ]; then
#        echo Removing contents of existing $SETUPIMAGEDIR
#        rm -f -r $SETUPIMAGEDIR/*
#    fi

    if [ ! -d $SETUPIMAGEDIR ]; then
        echo Making the $SETUPIMAGEDIR for preparing the setup
        mkdir -p $SETUPIMAGEDIR
    fi

	# Copying readmes
    if [ "$READMEFILE" != "" ] && [ -f $READMEFILE ]; then
        echo Copying readme.txt
        cp $READMEFILE $SETUPIMAGEDIR
#    else
#        echo "*** Warning - $READMEFILE not found"
    fi

    if [ "$LICENSEFILE" != "" ] && [ -f $LICENSEFILE ]; then
        echo Copying licence.txt
        cp $LICENSEFILE $SETUPIMAGEDIR
 #   else
 #       echo "*** Warning - $LICENSEFILE not found"
    fi

    if [ "$MAKEMANUAL" != "0" ]; then
        if [ -d $MANUALDIR ]; then
            cd $MANUALDIR
            make
        else
            echo "*** Warning - $MANUALDIR not found"
        fi
    fi

    rm -f $DESTDIR/wx*.zip
    rm -f $DESTDIR/*.htb
    rm -f $DESTDIR/ogl3*.zip
    rm -f $DESTDIR/contrib*.zip
    rm -f $DESTDIR/tex2rtf2*.zip
    rm -f $DESTDIR/mmedia*.zip
    rm -f $DESTDIR/jpeg*.zip
    rm -f $DESTDIR/tiff*.zip
    rm -f $DESTDIR/utils*.zip
    rm -f $DESTDIR/extradoc*.zip
    rm -f $DESTDIR/stc*.zip
    rm -f $DESTDIR/*-win32*.zip
    rm -f $DESTDIR/setup*.*
    rm -f $DESTDIR/*.txt
    rm -f $DESTDIR/make*

    if [ -d $DESTDIR/wx ]; then
        rm -f -r $DESTDIR/wx
    fi

    if [ ! -d $DESTDIR ]; then
        mkdir $DESTDIR
    fi
    if [ -d $DESTDIR/wxWidgets-$VERSION ]; then
        rm -f -r $DESTDIR/wxWidgets-$VERSION
    fi

    # Copy FAQ from wxWebSite CVS
    if [ ! -d $WEBFILES ]; then
        echo Error - $WEBFILES does not exist
        exit
    fi

    echo Copying FAQ and other files from $WEBFILES
    cp $WEBFILES/site/faq*.htm $APPDIR/docs/html
    cp $WEBFILES/site/platform.htm $APPDIR/docs/html
    cp $WEBFILES/site/i18n.htm $APPDIR/docs/html

    # Copy setup0.h files to setup.h
    # OS/2 always built with configure now
    # cp $APPDIR/include/wx/os2/setup0.h $APPDIR/include/wx/os2/setup.h
    cp $APPDIR/include/wx/msw/setup0.h $APPDIR/include/wx/msw/setup.h
    cp $APPDIR/include/wx/univ/setup0.h $APPDIR/include/wx/univ/setup.h

    # Do OS/2 spin
    if [ "$SPINOS2" = "1" ] || [ "$SPINALL" = "1" ]; then
        dospinos2
    fi

    # Do Mac spin
    if [ "$SPINMAC" = "1" ] || [ "$SPINALL" = "1" ]; then
        dospinmac
    fi

    # Do MSW spin
    if [ "$SPINMSW" = "1" ] || [ "$SPINALL" = "1" ]; then
        dospinmsw
    fi

    # Do wxBase spin
    if [ "$SPINBASE" = "1" ] || [ "$SPINALL" = "1" ]; then
        dospinbase
    fi

    # Do wxAll spin
    if [ "$SPINWXALL" = "1" ] || [ "$SPINALL" = "1" ]; then
        dospinwxall
    fi

    # Do docs spin
    if [ "$SPINDOCS" = "1" ] || [ "$SPINALL" = "1" ]; then
        dospindocs
    fi

    # Do misc files spin
    dospinmisc

    cp $APPDIR/docs/changes.txt $DESTDIR/changes-$VERSION.txt
    cp $APPDIR/docs/readme.txt $DESTDIR/readme-$VERSION.txt

    cp $APPDIR/docs/msw/readme.txt $DESTDIR/readme-msw-$VERSION.txt
    cp $APPDIR/docs/msw/install.txt $DESTDIR/install-msw-$VERSION.txt

    cp $APPDIR/docs/mac/readme.txt $DESTDIR/readme-mac-$VERSION.txt
    cp $APPDIR/docs/mac/install.txt $DESTDIR/install-mac-$VERSION.txt

    cp $APPDIR/docs/motif/readme.txt $DESTDIR/readme-motif-$VERSION.txt
    cp $APPDIR/docs/motif/install.txt $DESTDIR/install-motif-$VERSION.txt

    cp $APPDIR/docs/gtk/readme.txt $DESTDIR/readme-gtk-$VERSION.txt
    cp $APPDIR/docs/gtk/install.txt $DESTDIR/install-gtk-$VERSION.txt

    cp $APPDIR/docs/x11/readme.txt $DESTDIR/readme-x11-$VERSION.txt
#    cp $APPDIR/docs/x11/readme-nanox.txt $DESTDIR/readme-nanox-$VERSION.txt
    cp $APPDIR/docs/x11/install.txt $DESTDIR/install-x11-$VERSION.txt

    cp $APPDIR/docs/mgl/readme.txt $DESTDIR/readme-mgl-$VERSION.txt
    cp $APPDIR/docs/mgl/install.txt $DESTDIR/install-mgl-$VERSION.txt

    cp $APPDIR/docs/cocoa/readme.txt $DESTDIR/readme-cocoa-$VERSION.txt
    cp $APPDIR/docs/cocoa/install.txt $DESTDIR/install-cocoa-$VERSION.txt

    cp $APPDIR/docs/base/readme.txt $DESTDIR/readme-base-$VERSION.txt

    cp $APPDIR/docs/os2/install.txt $DESTDIR/install-os2-$VERSION.txt

    cp $APPDIR/docs/univ/readme.txt $DESTDIR/readme-univ-$VERSION.txt

    cp $APPDIR/docs/readme_vms.txt $DESTDIR/readme-vms-$VERSION.txt

    # cp $APPDIR/docs/motif/makewxmotif $DESTDIR/makewxmotif-$VERSION
    # cp $APPDIR/docs/gtk/makewxgtk $DESTDIR/makewxgtk-$VERSION
    
    # Time to regenerate the Inno Install script
    if [ "$INNO" != "0" ]; then
        dospinsetup
    fi
}

# Get the makefiles that aren't in CVS and unarchive them
getmakefiles()
{
    echo Getting eVC++ project files...
    curl http://biolpc22.york.ac.uk/pub/CVS_Makefiles/wx-mk-evcprj.zip --output /c/transit/wx-mk-evcprj.zip
    echo Getting Digital Mars makefiles...
    curl http://biolpc22.york.ac.uk/pub/CVS_Makefiles/wx-mk-dmars.zip --output /c/transit/wx-mk-dmars.zip
    echo Getting VC++ makefiles...
    curl http://biolpc22.york.ac.uk/pub/CVS_Makefiles/wx-mk-msvc.zip --output /c/transit/wx-mk-msvc.zip

    cd $APPDIR
    echo Unarchiving makefiles and project files...
    unzip -o -a /c/transit/wx-mk-evcprj.zip
    unzip -o -a /c/transit/wx-mk-dmars.zip
    unzip -o -a /c/transit/wx-mk-msvc.zip
    echo Done getting makefiles and project files.
}

# We can't use e.g. this:
# ls `cat $SRC/distrib/msw/makefile.rsp` zip -@ -u $DEST/wxWidgets-$VERSION-gen.zip
# because there's not enough space on the command line, plus we need to ignore the
# blank lines.
# So if we need to (not in this script so far) we do something like this instead:
# expandlines $SRC/setup/files.rsp temp.txt
# zip -@ `$CYGPATHPROG -w $DEST/archive.zip` < temp.txt

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

usage()
{
    echo "Usage: $PROGNAME [ options ]" 1>&2
    echo Options:
    echo "    --help            Display this help message"
    echo "    --upx             Compress executable with UPX"
    echo "    --no-upx          Do not compress executable with UPX"
    echo "    --inno            Build the setup.exe"
    echo "    --no-inno         Do not build the setup.exe"
    echo "    --wxmac           Build wxMac distribution"
    echo "    --wxmsw           Build wxMSW distribution"
    echo "    --wxos2           Build wxOS2 distribution"
    echo "    --wxall           Build wxAll zip distribution"
    echo "    --wxbase          Build wxBase zip distribution"
    echo "    --docs            Build docs archives"
    echo "    --all             Build all distributions (the default)"
    echo "    --getmakefiles    Get out-of-CVS makefiles and unarchive into the wxWidgets source tree"
    echo "    --verbose         Verbose zip operation"
    echo.
    echo Note that options only override settings in $SCRIPTDIR/setup.var.
    exit 1
}

# Process command line options.

for i in "$@"; do
    case "$i" in
	--inno) INNO=1 ;;
	--no-inno) INNO=0 ;;
	--upx) UPX=1 ;;
	--no-upx) UPX=0 ;;
	--wxmac) SPINMAC=1; SPINALL=0 ;;
	--wxmsw) SPINMSW=1; SPINALL=0 ;;
	--wxos2) SPINOS2=1; SPINALL=0 ;;
	--wxall) SPINWXALL=1; SPINALL=0 ;;
	--wxbase) SPINBASE=1; SPINALL=0 ;;
	--getmakefiles) GETMAKEFILES=1; SPINALL=0 ;;
	--docs) SPINDOCS=1; SPINALL=0 ;;
	--all) SPINALL=1 ;;
	--verbose) VERBOSE=1 ;;
	*)
	    usage
	    exit
	    ;;
    esac
done


doinit

if [ "$GETMAKEFILES" = "1" ]; then
    getmakefiles
    exit
fi

# findversion
makesetup


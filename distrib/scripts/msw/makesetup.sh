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
SPINOS2=0
SPINDOCS=0
SPINALL=0
SPINWXALL=0
SPINBASE=0
GETMAKEFILES=0
VERBOSE=0
ZIPFLAGS=

PROGNAME=$0
SCRIPTDIR=$WXWIN/distrib/scripts
. $SCRIPTDIR/msw/setup.var
. $SCRIPTDIR/utils.inc

MANIFESTDIR=$SCRIPTDIR/manifests
WEBFILES=c:/wx2dev/wxWebSite
if [ ! "$CYGPATH" = "" ]; then
  WEBFILES=`$CYGPATH "$WEBFILES"`
fi

# Set this to the required version
if [ "$VERSION" = "" ]; then
  VERSION=2.6.2
fi

getfilelist(){
  port=$1
  outfile=$2
  
  filelist="base.rsp"
  contribfiles="stc.rsp contrib.rsp ogl.rsp"
  utilsfiles="tex2rtf.rsp utils.rsp utilmake.rsp"
  commonfiles="generic.rsp generic_samples.rsp jpeg.rsp tiff.rsp xml.rsp deprecated.rsp makefile.rsp $utilsfiles $contribfiles"
  
  if [ ! $port = "base" ]; then
    filelist="$filelist $commonfiles" 
  fi 
  
  if [ $port = "msw" ] || [ $port = "all" ]; then
    filelist="$filelist msw.rsp univ.rsp vc.rsp mmedia.rsp wince.rsp dmc.rsp"
  fi
  
  if [ $port = "os2" ] || [ $port = "all" ]; then
    filelist="$filelist os2.rsp"
  fi
  
  if [ $port = "x11" ] || [ $port = "all" ]; then
    filelist="$filelist x11.rsp"
  fi
  
  if [ $port = "mgl" ] || [ $port = "all" ]; then
    filelist="$filelist mgl.rsp" 
  fi
  
  if [ $port = "gtk" ] || [ $port = "all" ]; then
    filelist="$filelist gtk.rsp"
  fi
  
  if [ $port = "cocoa" ] || [ $port = "all" ]; then
    filelist="$filelist cocoa.rsp"
  fi
  
  if [ $port = "motif" ] || [ $port = "all" ]; then
    filelist="$filelist motif.rsp"
  fi
  
  if [ $port = "mac" ] || [ $port = "all" ]; then
    filelist="$filelist mac.rsp"
  fi
  
  if [ $port = "all" ]; then
    filelist="$filelist palmos.rsp"
  fi
  
  tempfile="/tmp/wx$port.files.in"
  rm -f $tempfile 
  rm -f $outfile
  
  olddir=$PWD
  cd $MANIFESTDIR
  
  cat $filelist > $tempfile
  
  cd $APPDIR
  expandlines $tempfile $outfile
  
  cd $olddir
}

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

ziptotar()
{
    archive=$1
    dirname=$2
    changeto=$3

    pushd $changeto

    unzip $ZIPFLAGS $archive
    
    tar cfz $archive.tar.gz $dirname

    tar -cvf $dirname | bzip2 -9 > $archive.tar.bz2
    
    rm -rf $dirname

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
    echo Zipping OS/2...

    cd $APPDIR
    getfilelist "os2" /tmp/os2files
    
    # Zip up the complete wxOS2-xxx.zip file
    zip $ZIPFLAGS -@ $DESTDIR/wxOS2-$VERSION.zip < /tmp/os2files

    # Rearchive under wxWidgets-$VERSION
    if [ -d $DESTDIR/wxWidgets-$VERSION ]; then
        rm -f -r $DESTDIR/wxWidgets-$VERSION
    fi

    mkdir $DESTDIR/wxWidgets-$VERSION
    cd $DESTDIR/wxWidgets-$VERSION
    unzip $ZIPFLAGS ../wxOS2-$VERSION.zip

    echo Copying readme files...
    cp $APPDIR/docs/os2/install.txt INSTALL-OS2.txt

    cd $DESTDIR

    rm -f wxOS2-$VERSION.zip
    zip $ZIPFLAGS -r wxOS2-$VERSION.zip wxWidgets-$VERSION/*
}

dospinmsw()
{
    echo Zipping wxMSW...
    
    cd $APPDIR

    # now expand the wildcards to actual file names
    getfilelist "msw" /tmp/mswfiles

    # Create wxWidgets-$VERSION-win.zip which is used to create wxMSW
    echo Zipping individual components
    rm -f $DESTDIR/wxWidgets-$VERSION-win.zip
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-win.zip < /tmp/mswfiles
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
    
    cat $MANIFESTDIR/cw_mac.rsp $MANIFESTDIR/vc.rsp $MANIFESTDIR/x11.rsp $MANIFESTDIR/gtk.rsp $MANIFESTDIR/cocoa.rsp $MANIFESTDIR/motif.rsp $MANIFESTDIR/mac.rsp $MANIFESTDIR/mgl.rsp $MANIFESTDIR/os2.rsp $MANIFESTDIR/palmos.rsp | sort | uniq > /tmp/all.txt
    zip $ZIPFLAGS -@ -u $DESTDIR/wxWidgets-$VERSION-all.zip < /tmp/all.txt

    if [ -d $DESTDIR/wxWidgets-$VERSION ]; then
        rm -f -r $DESTDIR/wxWidgets-$VERSION
    fi

    mkdir $DESTDIR/wxWidgets-$VERSION
    cd $DESTDIR/wxWidgets-$VERSION
    unzip $ZIPFLAGS ../wxWidgets-$VERSION-all.zip

    cd $DESTDIR

    rm -f $DESTDIR/wxWidgets-$VERSION-all.zip
    zip $ZIPFLAGS -r wxWidgets-$VERSION.zip wxWidgets-$VERSION/*
}

dospinbase()
{
    cd $APPDIR

    echo Zipping wxBase...
    rm -f $DESTDIR/wxBase-$VERSION.zip
    expandlines $MANIFESTDIR/base.rsp /tmp/basefiles
    zip $ZIPFLAGS -@ $DESTDIR/wxBase-$VERSION.zip < /tmp/basefiles

    if [ -d $DESTDIR/wxWidgets-$VERSION ]; then
        rm -f -r $DESTDIR/wxWidgets-$VERSION
    fi

    mkdir $DESTDIR/wxWidgets-$VERSION
    cd $DESTDIR/wxWidgets-$VERSION
    unzip $ZIPFLAGS ../wxBase-$VERSION.zip

    echo Copying readme files...
    cp $APPDIR/docs/base/readme.txt README.txt

    cd $DESTDIR

    rm -f wxBase-$VERSION.zip
    zip $ZIPFLAGS -r wxBase-$VERSION.zip wxWidgets-$VERSION/*
}

dospindocs()
{
    cd $APPDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-DocSource.zip
    expandlines $MANIFESTDIR/docsrc.rsp /tmp/docsources
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-DocSource.zip < /tmp/docsources 
    rearchive wxWidgets-$VERSION-DocSource.zip wxWidgets-$VERSION $DESTDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-WinHelp.zip
    expandlines $MANIFESTDIR/wx_hlp.rsp /tmp/winhelp
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-WinHelp.zip < /tmp/winhelp
    rearchive wxWidgets-$VERSION-WinHelp.zip wxWidgets-$VERSION $DESTDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-HTML.zip
    expandlines $MANIFESTDIR/wx_html.rsp /tmp/htmldocs
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-HTML.zip < /tmp/htmldocs
    rearchive wxWidgets-$VERSION-HTML.zip wxWidgets-$VERSION $DESTDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-PDF.zip
    expandlines $MANIFESTDIR/wx_pdf.rsp /tmp/pdfdocs
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-PDF.zip < /tmp/pdfdocs
    rearchive wxWidgets-$VERSION-PDF.zip wxWidgets-$VERSION $DESTDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-PDF.tar.gz
    tar zcf /c/wx2dev/wxWindows/deliver/wxWidgets-$VERSION-PDF.tar.gz docs/pdf/*.pdf
    rearchivetar wxWidgets-$VERSION-PDF.tar.gz wxWidgets-$VERSION /c/wx2dev/wxWindows/deliver

    echo Creating $DESTDIR/wxWidgets-$VERSION-HTB.zip
    expandlines $MANIFESTDIR/wx_htb.rsp /tmp/htbdocs
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-HTB.zip < /tmp/htbdocs
    rearchive wxWidgets-$VERSION-HTB.zip wxWidgets-$VERSION $DESTDIR

    echo Creating $DESTDIR/wxWidgets-$VERSION-HTMLHelp.zip
    expandlines $MANIFESTDIR/wx_chm.rsp /tmp/chmdocs
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-HTMLHelp.zip < /tmp/chmdocs 
    rearchive wxWidgets-$VERSION-HTMLHelp.zip wxWidgets-$VERSION $DESTDIR

    # Add Linuxy docs to a separate archive to be transported to Linux for the
    # Linux-based releases
    echo Creating $DESTDIR/wxWidgets-$VERSION-LinuxDocs.zip
    
    cat $MANIFESTDIR/wx_html.rsp $MANIFESTDIR/wx_pdf.rsp $MANIFESTDIR/wx_htb.rsp > /tmp/linuxdocs.in
    expandlines /tmp/linuxdocs.in /tmp/linuxdocs
    
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-LinuxDocs.zip < /tmp/linuxdocs

    # PDF/HTML docs that should go into the Windows setup because
    # there are no WinHelp equivalents
    echo Creating $DESTDIR/wxWidgets-$VERSION-ExtraDoc.zip
    expandlines $SCRIPT/extradoc.rsp /tmp/extradocs
    zip $ZIPFLAGS -@ $DESTDIR/wxWidgets-$VERSION-ExtraDoc.zip < /tmp/extradocs
    rearchive wxWidgets-$VERSION-ExtraDoc.zip wxWidgets-$VERSION $DESTDIR
}


dospinport(){
    port=$1
    
    if [ $port != "all" ]; then
        portname="`echo $port|tr '[a-z]' '[A-Z]'`"    
    else
        portname="wxWidgets"
    fi
    
    echo "Zipping wx$portname..."

    cd $APPDIR
    portfiles="/tmp/wx$port.files"
    getfilelist "$port" "$portfiles"

    zip $ZIPFLAGS -@ $DESTDIR/wx$portname-$VERSION.zip < $portfiles 
    zip $ZIPFLAGS -g $DESTDIR/wx$portname-$VERSION.zip LICENSE.txt COPYING.LIB CHANGES.txt README.txt
    
    if [ $port = "msw" ] || [ $port = "all" ]; then
        zip $ZIPFLAGS -g $DESTDIR/wx$portname-$VERSION.zip README-MSW.txt INSTALL-MSW.txt        
    fi
    
    if [ $port = "os2" ] || [ $port = "all" ]; then
        zip $ZIPFLAGS -g $DESTDIR/wx$portname-$VERSION.zip INSTALL-OS2.txt          
    fi
    
    # put all files in a wxWidgets-$VERSION subdir in the zip archive
    rearchive wx$portname-$VERSION.zip wxWidgets-$VERSION $DESTDIR
}

dospininstaller()
{
    cd $DESTDIR

    rm -f -r wxWidgets-$VERSION

    echo Unzipping the Windows files into wxWidgets-$VERSION

    mkdir -p wxWidgets-$VERSION

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


    # Disabled for now - Now cp some binary files to 'bin'
    if [ ! -d bin ]; then
        mkdir bin
    fi
    
    #cp $APPDIR/bin/tex2rtf.exe bin
    #cp $APPDIR/bin/tex2rtf.chm bin
    #cp $APPDIR/bin/widgets.exe bin
    #cp $APPDIR/bin/life.exe bin
    #cp $APPDIR/demos/life/breeder.lif bin
    #cp $APPDIR/docs/htmlhelp/tex2rtf.chm bin

    if [ ! -d docs/pdf ]; then
        mkdir docs/pdf
    fi
    #cp $APPDIR/docs/pdf/wxTutorial.pdf docs/pdf

    # Make wxMSW-xxx.zip
    cd $DESTDIR
    zip $ZIPFLAGS -r wxMSW-$VERSION.zip wxWidgets-$VERSION/*
    cd wxWidgets-$VERSION

    if [ "$INNO" != "0" ]; then
        echo Generating $SETUPSCRIPTNAME
        rm -f $SETUPSCRIPTNAME
    
        sh $SCRIPTDIR/msw/makeinno.sh $SETUPIMAGEDIR $INNOTOP $INNOBOTTOM $SETUPSCRIPTNAME
    
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

    fi
    # echo Putting all the setup files into a single zip archive
    # zip wxMSW-$VERSION-setup.zip readme-$VERSION.txt setup*.*

    rm -f wxWidgets-$VERSION-ExtraDoc.zip
    rm -f wxWidgets-$VERSION-DocSource.zip
    rm -f wxWidgets-$VERSION-LinuxDocs.zip

    echo If you saw no warnings or errors, $APPTITLE was successfully spun.
    echo
}

makesetup()
{

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
    #if [ ! -d $WEBFILES ]; then
    #    echo Error - $WEBFILES does not exist
    #    exit
    #fi

    echo Copying FAQ and other files from $WEBFILES
    cp $WEBFILES/site/faq*.htm $APPDIR/docs/html
    cp $WEBFILES/site/platform.htm $APPDIR/docs/html
    cp $WEBFILES/site/i18n.htm $APPDIR/docs/html
    
    echo Copying readme files...
    cp $APPDIR/docs/msw/readme.txt README-MSW.txt
    cp $APPDIR/docs/msw/install.txt INSTALL-MSW.txt
    cp $APPDIR/docs/os2/install.txt INSTALL-OS2.txt
    cp $APPDIR/docs/licence.txt LICENCE.txt
    cp $APPDIR/docs/lgpl.txt COPYING.LIB
    cp $APPDIR/docs/changes.txt CHANGES.txt
    cp $APPDIR/docs/readme.txt README.txt
    
    # Copy setup0.h files to setup.h
    # OS/2 always built with configure now
    # cp $APPDIR/include/wx/os2/setup0.h $APPDIR/include/wx/os2/setup.h
    cp $APPDIR/include/wx/msw/setup0.h $APPDIR/include/wx/msw/setup.h
    cp $APPDIR/include/wx/univ/setup0.h $APPDIR/include/wx/univ/setup.h

    # Do OS/2 spin
    if [ "$SPINOS2" = "1" ] || [ "$SPINALL" = "1" ]; then
        dospinport "os2" #dospinos2
    fi

    # Do MSW spin
    if [ "$SPINMSW" = "1" ] || [ "$SPINALL" = "1" ]; then
        dospinport "msw" #dospinmsw
    fi

    # Do wxBase spin
    if [ "$SPINBASE" = "1" ] || [ "$SPINALL" = "1" ]; then
        dospinport "base" #dospinbase
    fi

    # Do wxAll spin
    if [ "$SPINWXALL" = "1" ] || [ "$SPINALL" = "1" ]; then
        dospinport "all" #dospinwxall
    fi

    if [ "$SPINALL" = "1" ]; then
        dospinport "mgl"
        dospinport "gtk"
        dospinport "x11"
        dospinport "motif"
        dospinport "mac"
        dospinport "cocoa"
        #dospinwxall
    fi


    # Do docs spin
    if [ "$SPINDOCS" = "1" ] || [ "$SPINALL" = "1" ]; then
        dospindocs
    fi

    docopydocs $APPDIR $DESTDIR
    
    # Time to regenerate the Inno Install script
    dospininstaller
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
    echo Note that options only override settings in $SCRIPTDIR/msw/setup.var.
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


#!/bin/sh

# Make a distribution of an application on MSW.
# Example:
# ../distrib/msw/makesetup.sh --wxmsw --verbose &> log

# If your zip accepts Cygwin-style paths, then
# use cygpath, else substitute echo
CYGPATHPROG=cygpath
#CYGPATHPROG=echo

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
##SCRIPTDIR=$WXWIN/distrib/scripts
. $SCRIPTDIR/utils.inc

MANIFESTDIR=$WXWIN/distrib/scripts/manifests
WEBFILES=$WXWIN/../wxWebSite
if [ ! "$CYGPATH" = "" ]; then
  WEBFILES=`$CYGPATH "$WEBFILES"`
fi

# Set this to the required version
if [ "$VERSION" = "" ]; then
  VERSION=2.7.0
fi

getfilelist(){
  port=$1
  outfile=$2
  
  filelist="base.rsp"
  contribfiles="stc.rsp contrib.rsp ogl.rsp"
  utilsfiles="tex2rtf.rsp utils.rsp utilmake.rsp"
  commonfiles="generic.rsp jpeg.rsp tiff.rsp xml.rsp deprecated.rsp makefile.rsp $utilsfiles $contribfiles"
  
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
  
  if [ $port = "motif" ] || [ $port = "all" ]; then
    filelist="$filelist motif.rsp"
  fi
  
  if [ $port = "mac" ] || [ $port = "all" ]; then
    filelist="$filelist mac.rsp cocoa.rsp"
  fi
  
  if [ $port = "all" ]; then
    filelist="$filelist gtk1.rsp palmos.rsp docsrc.rsp"
  fi
  
  tempfile="/tmp/wx$port.files.in"
  rm -f $outfile
  
  olddir=$PWD
  cd $MANIFESTDIR
  
  cat $filelist > $outfile
  
  cd $olddir
}

copyfilelist(){
    FILELIST=$1
    APPDIR=$2
    DESTDIR=$3
    
    for line in `cat $FILELIST` ; do
      if [ "$line" != "" ]; then
        subdir=`dirname $line`
        mkdir -p $DESTDIR/$subdir
        cp -rf $APPDIR/$line $DESTDIR/$subdir 
      fi
    done
}

doinit()
{
    if [ "$VERBOSE" != "1" ]; then
        ZIPFLAGS=-q
    fi
}

dospinport(){
    port=$1
    
    case "$port" in
       all)
          portname=Widgets;;
       base)
          portname=Base;;
       motif)
          portname=Motif;;
       mac)
          portname=Mac;;
       *)
          portname=echo $port | tr [a-z] [A-Z];;
    esac
    
    echo "Creating wx$portname distribution..."

    cd $APPDIR
    portfiles="/tmp/wx$port.files"
    getfilelist "$port" "$portfiles"
    
    TMPFILESDIR=/tmp/wx$port/wx$portname-$VERSION
    rm -rf $TMPFILESDIR
    mkdir -p $TMPFILESDIR

    copyfilelist $portfiles $APPDIR $TMPFILESDIR

    if [ $port = "msw" ]; then
        FILES=`find . -type f \( -path '*/CVS/*' -prune -o -exec ${SCRIPTDIR}/is_text.sh {} \; -print \)`
        echo "$FILES" > /tmp/textfiles
    fi

    pushd /tmp/wx$port
    # use DOS line endings for text files for MSW archives.
    if [ $port = "msw" ]; then
        pushd /tmp/wx$port/wx$portname-$VERSION
        for file in `cat /tmp/textfiles`; do
            unix2dos $file
        done
        popd
    fi
    echo "Creating wx$portname-$VERSION.zip..."
    zip $ZIPFLAGS -r -9 $APPDIR/deliver/wx$portname-$VERSION.zip .
    echo "Creating wx$portname-$VERSION.tar.gz..."
    tar czf $APPDIR/deliver/wx$portname-$VERSION.tar.gz wx$portname-$VERSION
    echo "Creating wx$portname-$VERSION.tar.bz2..."
    tar ch wx$portname-$VERSION | bzip2 -f9 > $APPDIR/deliver/wx$portname-$VERSION.tar.bz2
    popd
    rm -rf /tmp/wx$port
    rm ${portfiles}
#    rm /tmp/textfiles
}

prepareforrelease()
{
    pushd $APPDIR
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

    cp $APPDIR/docs/mgl/readme.txt $APPDIR/readme-mgl.txt
    cp $APPDIR/docs/mgl/install.txt $APPDIR/install-mgl.txt

    cp $APPDIR/docs/x11/readme.txt $APPDIR/readme-x11.txt
    cp $APPDIR/docs/x11/install.txt $APPDIR/install-x11.txt

    cp $APPDIR/docs/motif/readme.txt $APPDIR/readme-motif.txt
    cp $APPDIR/docs/motif/install.txt $APPDIR/install-motif.txt

    cp $APPDIR/docs/gtk/readme.txt $APPDIR/readme-gtk.txt
    cp $APPDIR/docs/gtk/install.txt $APPDIR/install-gtk.txt

    cp $APPDIR/docs/mac/readme.txt $APPDIR/readme-mac.txt
    cp $APPDIR/docs/mac/install.txt $APPDIR/install-mac.txt

    cp $APPDIR/docs/cocoa/readme.txt $APPDIR/readme-cocoa.txt
    cp $APPDIR/docs/cocoa/install.txt $APPDIR/install-cocoa.txt
    
    # Now delete a few files that are unnecessary
    rm -f BuildCVS.txt descrip.mms
    rm -f setup.h_vms
    rm -f docs/html/wxbook.htm docs/html/roadmap.htm
    rm -f -r contrib/docs/latex/ogl
    rm -f src/mingegcs.bat
#    rm -f *.spec
    rm -f src/gtk/descrip.mms src/motif/descrip.mms
    
    # Copy setup0.h files to setup.h
    # OS/2 always built with configure now
    # cp $APPDIR/include/wx/os2/setup0.h $APPDIR/include/wx/os2/setup.h
    cp $APPDIR/include/wx/msw/setup0.h $APPDIR/include/wx/msw/setup.h
    cp $APPDIR/include/wx/univ/setup0.h $APPDIR/include/wx/univ/setup.h
    
    # Make MSW project files always have DOS line endings.
    unix2dos `cat $MANIFESTDIR/vc.rsp` 
    
    popd
}

usage()
{
    echo "Usage: $PROGNAME [ options ]" 1>&2
    echo Options:
    echo "    --help            Display this help message"
    echo "    --wxmac           Build wxMac distribution"
    echo "    --wxmsw           Build wxMSW distribution"
    echo "    --wxos2           Build wxOS2 distribution"
    echo "    --wxall           Build wxAll zip distribution"
    echo "    --wxbase          Build wxBase zip distribution"
    echo "    --all             Build all distributions (the default)"
    echo "    --verbose         Verbose zip operation"
    exit 1
}

# Process command line options.

for i in "$@"; do

    case "$i" in
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
prepareforrelease

# Do OS/2 spin
if [ "$SPINOS2" = "1" ] || [ "$SPINALL" = "1" ]; then
    dospinport "os2"
fi

# Do MSW spin
if [ "$SPINMSW" = "1" ] || [ "$SPINALL" = "1" ]; then
    dospinport "msw"
fi

# Do wxBase spin
if [ "$SPINBASE" = "1" ] || [ "$SPINALL" = "1" ]; then
    dospinport "base"
fi

# Do wxAll spin
if [ "$SPINWXALL" = "1" ] || [ "$SPINALL" = "1" ]; then
    dospinport "all" 
fi

if [ "$SPINALL" = "1" ]; then
    dospinport "mgl"
    dospinport "gtk"
    dospinport "x11"
    dospinport "motif"
    dospinport "mac"
fi


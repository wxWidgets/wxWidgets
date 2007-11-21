#!/bin/sh

# Create an Inno Setup installer on MSW. It requires MSYS.
#
# Example:
#
# distrib/msw/createsetup.sh
#
# Prerequisites:
#
# (1) Change VERSION to the desired wxWidgets version.
# (2) Change APPDIR to the root of a wxWidgets distribution containing
#     this script.
# (3) Change DESTDIR (if necessary) to a directory for creating the
#     the installer.
# (4) Change SETUPCOMPILER to where Inno Setup is installed.
# (6) Make sure 'unzip' is in your path.
# (7) Make sure wxMSW-x.y.z.zip and wxWidgets-x.y.z-CHM.zip are in
#     the directory $DESTDIR.
# (8) Run createsetup.sh.
#
# The necessary script files are:
#
# distrib/msw/createsetup.sh                  ; The main script
# distrib/msw/wxwidgets_in.iss                ; The input for making the .iss
#

#-------------------------------------------------------------------------
# The version
#-------------------------------------------------------------------------

VERSION=2.7.2

#-------------------------------------------------------------------------
# The application title
#-------------------------------------------------------------------------

APPTITLE="wxWidgets"

#-------------------------------------------------------------------------
# The application author (copyright holder)
#-------------------------------------------------------------------------

AUTHOR="The wxWidgets Team"

#-------------------------------------------------------------------------
# The application vendor (organisation)
#-------------------------------------------------------------------------

VENDOR="wxWidgets"

#-------------------------------------------------------------------------
# The top-level directory of the application source tree
#-------------------------------------------------------------------------

APPDIR=c:/wx2dev/wxWindows

#-------------------------------------------------------------------------
# The destination directory of the setup
#-------------------------------------------------------------------------

DESTDIR=$APPDIR/deliver

#-------------------------------------------------------------------------
# The temporary image directory to use when preparing the setup
#-------------------------------------------------------------------------

SETUPIMAGEDIR=$DESTDIR/wxWidgets-$VERSION

#-------------------------------------------------------------------------
# The location of the setup script
#-------------------------------------------------------------------------

SETUPSCRIPTNAME=$APPDIR/distrib/msw/wxwidgets.iss
SETUPSCRIPTNAME_IN=$APPDIR/distrib/msw/wxwidgets_in.iss

#-------------------------------------------------------------------------
# The location of the zip files to be used to create the setup
#-------------------------------------------------------------------------

WXMSWARCHIVE=$DESTDIR/wxMSW-$VERSION.zip
WXCHMARCHIVE=$DESTDIR/wxWidgets-$VERSION-CHM.zip

#-------------------------------------------------------------------------
# The locations of the readme and license files
#-------------------------------------------------------------------------

READMEFILE=$SETUPIMAGEDIR/docs/readme.txt
READMEAFTERFILE=$SETUPIMAGEDIR/docs/msw/install.txt
LICENSEFILE=$SETUPIMAGEDIR/docs/licence.txt

#-------------------------------------------------------------------------
# The location of the setup compiler
#-------------------------------------------------------------------------

SETUPCOMPILER="c:/Program Files/Inno Setup 5/compil32.exe"

#-------------------------------------------------------------------------
# Misc
#-------------------------------------------------------------------------

# If your zip accepts Cygwin-style paths, then
# use cygpath, else substitute echo
CYGPATHPROG=cygpath
#CYGPATHPROG=echo
PROGNAME=$0
SCRIPTDIR=$WXWIN/distrib/msw
ZIPFLAGS=

# Seds the file represented by the 1st arg, using the expression passed as the 2nd arg.
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

# Do the actual spin.

dospinsetup()
{
    mkdir -p $DESTDIR
    cd $DESTDIR
    
    echo Now in `pwd`
    
    rm -f -r wxWidgets-$VERSION
    rm -f -r wxMSW-$VERSION

    echo Unzipping the wxMSW files...
    
    unzip $ZIPFLAGS $WXMSWARCHIVE

    # This 'pause' is only necessary because on my system, the following 'mv' seems to fail
    # without it.
    echo About to rename wxMSW-$VERSION to wxWidgets-$VERSION. Press Return to continue.
    read ch
    
    mv wxMSW-$VERSION wxWidgets-$VERSION
    
    if [ ! -d wxWidgets-$VERSION ]; then
    	echo *** Could not find wxWidgets-$VERSION: exiting.
    	exit
    fi

    echo Unzipping the CHM files...
    
    cd wxWidgets-$VERSION
    unzip $ZIPFLAGS $WXCHMARCHIVE
    
    echo Copying readme files...
    cp docs/msw/readme.txt README-MSW.txt
    cp docs/msw/install.txt INSTALL-MSW.txt
    cp docs/licence.txt LICENCE.txt
    cp docs/lgpl.txt COPYING.LIB
    cp docs/changes.txt CHANGES.txt
    cp docs/readme.txt README.txt

    # Now cp some binary files to 'bin'
    # TODO
#    if [ ! -d bin ]; then
#        mkdir bin
#    fi
#    cp $APPDIR/bin/tex2rtf.exe bin
#    cp $APPDIR/bin/tex2rtf.chm bin
#    cp $APPDIR/bin/widgets.exe bin
#    cp $APPDIR/bin/life.exe bin
#    cp $APPDIR/demos/life/breeder.lif bin
#    cp $APPDIR/docs/htmlhelp/tex2rtf.chm bin

    # Now replace %VERSION% with the real application version, and other
    # variables
    
    cp $SETUPSCRIPTNAME_IN $SETUPSCRIPTNAME
    
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

    doreplace $SETUPSCRIPTNAME "s/%APPTITLE%/$APPTITLE/g"

    unix2dosname $SETUPIMAGEDIR
    doreplace $SETUPSCRIPTNAME "s;%SOURCEDIR%;$RETVALUE;g"

    unix2dosname $DESTDIR
    doreplace $SETUPSCRIPTNAME "s;%OUTPUTDIR%;$RETVALUE;g"

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

    echo If you saw no warnings or errors, $APPTITLE was successfully spun.
    echo
}

# Get the archives
getarchives()
{
    echo Getting archives...
    cd $DESDIR
    curl http://biolpc22.york.ac.uk/pub/$VERSION/wxMSW-$VERSION.zip
    curl http://biolpc22.york.ac.uk/pub/$VERSION/wxWidgets-$VERSION-CHM.zip
    
    # TODO
    #curl http://biolpc22.york.ac.uk/pub/$VERSION/wxWidgets-Windows-Binaries.zip

    echo Done getting archives.
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
    echo "    --verbose         Verbose zip operation"
    echo.
    echo Note that options only override settings in $SCRIPTDIR/setup.var.
    exit 1
}

# Process command line options.

for i in "$@"; do
    case "$i" in
	--getarchives) GETARCHIVES=1 ;;
	--verbose) VERBOSE=1 ;;
	*)
	    usage
	    exit
	    ;;
    esac
done


doinit

if [ "$GETARCHIVES" = "1" ]; then
    getarchives
fi

dospinsetup


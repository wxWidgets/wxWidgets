#!/bin/sh

# Make a distribution of an application.

# If your zip accepts Cygwin-style paths, then
# use cygpath, else substitute echo
CYGPATHPROG=cygpath
#CYGPATHPROG=echo

PROGNAME=$0
SCRIPTDIR=`pwd`

. $SCRIPTDIR/setup.var

# Set this to the required version
VERSION=

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

findversion()
{
	echo "#include <stdio.h>" > /tmp/appver.c
	echo "#include \"$VERSIONSYMBOLFILE\"" >> /tmp/appver.c
	echo "int main() { printf(\"%.2f\", $VERSIONSYMBOL); }" >> /tmp/appver.c
	gcc /tmp/appver.c -I$APPDIR -o /tmp/appver
    VERSION=`/tmp/appver`
	rm -f /tmp/appver /tmp/appver.c
}

makesetup()
{
    if [ -d $SETUPIMAGEDIR ]; then
        echo Removing existing $SETUPIMAGEDIR
        rm -f -r $SETUPIMAGEDIR
    fi

    echo Making the $SETUPIMAGEDIR for preparing the setup
    mkdir -p $SETUPIMAGEDIR

    if [ -f $READMEFILE ]; then
        echo Copying readme.txt
        cp $READMEFILE $SETUPIMAGEDIR
    else
        echo "*** Warning - $READMEFILE not found"
    fi

    if [ -f $LICENSEFILE ]; then
        echo Copying licence.txt
        cp $LICENSEFILE $SETUPIMAGEDIR
    else
        echo "*** Warning - $LICENSEFILE not found"
    fi

	cp $APPDIR/docs/gpl.txt $SETUPIMAGEDIR
	cp $APPDIR/docs/lgpl.txt $SETUPIMAGEDIR
	cp $APPDIR/docs/licendoc.txt $SETUPIMAGEDIR

	echo Copying sample configuration settings file...
	cp $APPDIR/configs/wxwin250.wxs $SETUPIMAGEDIR

    if [ "$RESOURCESFILE" != "" ] && [ "$RESOURCESDIR" != "" ]; then
        if [ -d $RESOURCESDIR ]; then
            cd $RESOURCESDIR
            echo Compiling resource file $RESOURCESFILE
            zip $SETUPIMAGEDIR/$RESOURCESFILE *
        else
            echo "*** Warning - $RESOURCESDIR directory not found"
        fi
    fi

    if [ "$MAKEMANUAL" != "0" ]; then
        if [ -d $MANUALDIR ]; then
            cd $MANUALDIR
            make

            for EACHFILE in $MANUALFILES ; do
                if [ -f $EACHFILE ]; then
                    echo Copying $EACHFILE
                    cp $EACHFILE $SETUPIMAGEDIR
                else
                    echo "*** Warning - $EACHFILE not found"
                fi
            done
        else
            echo "*** Warning - $MANUALDIR not found"
        fi
    fi

    if [ -f $WXMANUALCHM ]; then
        echo Copying wxWindows manual $WXMANUALCHM
        cp $WXMANUALCHM $SETUPIMAGEDIR
    else
        echo "*** Warning - $WXMANUALCHM not found"
    fi

    if [ -f $APPBINARY ]; then
        echo Copying binary $APPBINARY
        cp $APPBINARY $SETUPIMAGEDIR
    else
        echo "*** Error - $APPBINARY not found"
        exit 1
    fi

    if [ "$UPX" != "0" ]; then
        echo Compressing binary
        upx $SETUPIMAGEDIR/`basename $APPBINARY`
    fi
    
    # Time to regenerate the Inno Install script
    if [ "$INNO" != "0" ]; then
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
    fi

    rm -f $DESTDIR/setup*.*

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
    mv setup.exe $APPNAME-$VERSION-setup.exe

    echo If you saw no warnings or errors, $APPTITLE was successfully spun.
    echo
}

# We can't use e.g. this:
# ls `cat $SRC/distrib/msw/makefile.rsp` zip -@ -u $DEST/wxWindows-$VERSION-gen.zip
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
	*)
	    usage
	    exit
	    ;;
    esac
done

findversion
makesetup


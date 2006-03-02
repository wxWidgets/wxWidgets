#! /bin/sh
# Make an Inno Setup distribution list, where files and dirs are represented by
# sections like this:
# [Dirs]
#     Name: {app}\backgrounds
# 
# [Files]
#     Source: C:\program\setup\about.htm; DestDir: {app}\; DestName: about.htm
#
#
# Usage: makeinno.sh sourcedir inno-topfile inno-bottomfile destfile
# For example: makeinno.sh c:/project/allfiles c:/project/innotop.txt c:/project/innobott.txt c:/project/project.iss
#

PROGNAME=$0
SOURCEDIR=$1
TOPFILE=$2
BOTTOMFILE=$3
INNOFILE=$4
TEMPDIR=/tmp

dochecks()
{
    if [ "$SOURCEDIR" = "" ] || [ "$TOPFILE" = "" ] || [ "$BOTTOMFILE" = "" ] || [ "$INNOFILE" = "" ] ; then
        usage
    fi

    if [ ! -d $SOURCEDIR ]; then
        echo "Sorry, the source directory $SOURCEDIR does not exist."
        usage
    fi

    if [ ! -f $TOPFILE ]; then
        echo "Sorry, the Inno Setup header $TOPFILE does not exist."
        usage
    fi

    if [ ! -f $BOTTOMFILE ]; then
        echo "Sorry, the Inno Setup header $BOTTOMFILE does not exist."
        usage
    fi

    if [ ! -d $TEMPDIR ]; then
        mkdir $TEMPDIR
    fi
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

generateinno()
{
    # SRCDIR=`cygpath -u $SRCDIR`
    # DESTDIR=`cygpath -u $DESTDIR`
    # TEMPDIR=`cygpath -u $TEMP`


    # Generate a list of all files in the distribution.
    # We pass the output through sed in order to remove the preceding "./"
    cd $SOURCEDIR
    find . -print | sed -e "s/\.\\///g" > $TEMPDIR/files1.tmp

    echo "[Dirs]" > $TEMPDIR/files2.tmp

    for line in `cat $TEMPDIR/files1.tmp` ; do

        # If a directory, add to file
        if [ -d $line ] ; then
            # The relative path
            # TODO: make into DOS filename form
            #line2=`cygpath -w $line`
            line2=$line

            echo "  Name: {app}\\"$line2 >> $TEMPDIR/files2.tmp
        fi
    done

    echo "" >> $TEMPDIR/files2.tmp
    echo "[Files]" >> $TEMPDIR/files2.tmp

    for line in `cat $TEMPDIR/files1.tmp` ; do

        # If not a directory, add to file
        if [ ! -d $line ] ; then
            # The relative path
            # TODO: make into DOS filename form
            #line2=`cygpath -w $line`
            line2=$line

            # The absolute path
            # TODO: make into DOS filename form
            #line1=`cygpath -w $SOURCEDIR`"\\"$line2
            line1=$SOURCEDIR"\\"$line2
            #pathonly=`find $line -printf "%h"`
            pathonly=`dirname $line`

            echo "  Source: "$line1"; DestDir: {app}\\"$pathonly >> $TEMPDIR/files2.tmp
        fi
    done

    echo "" >> $TEMPDIR/files2.tmp

    doreplace $TEMPDIR/files2.tmp "s/\//\\\/g"

    # Concatenate the 3 sections
    cat $TOPFILE $TEMPDIR/files2.tmp $BOTTOMFILE > $INNOFILE

    rm -f $TEMPDIR/files1.tmp
}

usage()
{
    echo Usage: $PROGNAME sourcedir inno-topfile inno-bottomfile destfile
    echo For example: $PROGNAME c:/project/allfiles c:/project/innotop.txt c:/project/innobott.txt c:/project/project.iss
    echo Remember to use paths of the form c:/thing rather than /c/thing.
    exit 1
}

dochecks
generateinno



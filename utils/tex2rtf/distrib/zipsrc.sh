#!/bin/sh
# Zip up StoryLines source

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

CAFEDIR=$CAFE
STDIR=$CAFE/storylines
COMMONDIR=$CAFE/common

cd $CAFE
expandlines $STDIR/distrib/src.rsp temp.txt
zip -@ $CAFEDIR/deliver/storylines_src.zip < temp.txt
expandlines $CAFEDIR/distrib/src.rsp temp.txt
zip -u -@ $CAFEDIR/deliver/storylines_src.zip < temp.txt
rm temp.txt

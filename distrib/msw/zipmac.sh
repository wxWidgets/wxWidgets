#!/bin/sh

# Zip up an external, generic + Windows distribution of wxWidgets 2
# Usage: zipmac.sh dest-dir
# (set the WXWIN variable first so it knows where to take the sources from)

if [ "$OS" = "Windows_NT" ]; then
  echo NT
  CYGPATHPROGW="cygpath -w"
  CYGPATHPROGU="cygpath -u"
else
  echo Not NT
  CYGPATHPROGW=echo
  CYGPATHPROGU=echo
fi

SRC=`$CYGPATHPROGU $WXWIN`
DEST=$1

# Set this to the required version
VERSION=2.3.0

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

# Remove all existing files
rm -f  $DEST/wx*.zip

if [ ! -d "$DEST/wx" ]; then
    rm -f -r $DEST/wx
fi


cd $SRC
echo Zipping...

expandlines $SRC/distrib/msw/generic.rsp temp.txt
zip -@ `$CYGPATHPROGW $DEST/wxMac-$VERSION.zip` < temp.txt

expandlines $SRC/distrib/msw/mac.rsp temp.txt
zip -u -@ `$CYGPATHPROGW $DEST/wxMac-$VERSION.zip` < temp.txt

expandlines $SRC/distrib/msw/cocoa.rsp temp.txt
zip -u -@ `$CYGPATHPROGW $DEST/wxMac-$VERSION.zip` < temp.txt

expandlines $SRC/distrib/msw/cw.rsp temp.txt
zip -u -@ `$CYGPATHPROGW $DEST/wxMac-$VERSION.zip` < temp.txt

expandlines $SRC/distrib/msw/tiff.rsp temp.txt
zip -u -@ `$CYGPATHPROGW $DEST/wxMac-$VERSION.zip` < temp.txt

expandlines $SRC/distrib/msw/jpeg.rsp temp.txt
zip -u -@ `$CYGPATHPROGW $DEST/wxMac-$VERSION.zip` < temp.txt

expandlines $SRC/distrib/msw/tex2rtf.rsp temp.txt
zip -u -@ `$CYGPATHPROGW $DEST/wxMac-$VERSION.zip` < temp.txt

expandlines $SRC/distrib/msw/ogl.rsp temp.txt
zip -u -@ `$CYGPATHPROGW $DEST/wxMac-$VERSION.zip` < temp.txt

expandlines $SRC/distrib/msw/xml.rsp temp.txt
zip -u -@ `$CYGPATHPROGW $DEST/wxMac-$VERSION.zip` < temp.txt

expandlines $SRC/distrib/msw/wx_html.rsp temp.txt
zip -@ `$CYGPATHPROGW $DEST/wxWidgets-$VERSION-HTML.zip` < temp.txt

rm -f temp.txt

echo wxWidgets archived.


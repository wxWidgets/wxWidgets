#!/bin/sh
# Zip up Tex2RTF source

VERSION=-$1
if [ "$VERSION" = "-" ] ; then
  VERSION=""
fi

TEX2RTFDIR=`pwd`/..

rm -f $TEX2RTFDIR/deliver/tex2rtf$VERSION-source.zip
rm -f $TEX2RTFDIR/deliver/tex2rtf$VERSION-source.tar.gz

mkdir $TEX2RTFDIR/tex2rtf$VERSION-source

cd $TEX2RTFDIR
tar -c `cat $TEX2RTFDIR/distrib/src.rsp` | (cd tex2rtf$VERSION-source ; tar -x)

# Make a zip archive
zip -9 -r $TEX2RTFDIR/deliver/tex2rtf$VERSION-source.zip tex2rtf$VERSION-source

# Now make a tar archive
tar cf $TEX2RTFDIR/deliver/tex2rtf$VERSION-source.tar tex2rtf$VERSION-source
gzip -9 $TEX2RTFDIR/deliver/tex2rtf$VERSION-source.tar
tar cf $TEX2RTFDIR/deliver/tex2rtf$VERSION-source.tar tex2rtf$VERSION-source
bzip2 -9 $TEX2RTFDIR/deliver/tex2rtf$VERSION-source.tar


rm -rf $TEX2RTFDIR/tex2rtf$VERSION-source

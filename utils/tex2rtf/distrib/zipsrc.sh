#!/bin/sh
# Zip up Tex2RTF source

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

TEX2RTFDIR=`pwd`/..

rm -f $TEX2RTFDIR/deliver/tex2rtf-source.zip
rm -f $TEX2RTFDIR/deliver/tex2rtf-source.tar.gz

# Make a zip archive
cd $TEX2RTFDIR
expandlines $TEX2RTFDIR/distrib/src.rsp temp.txt
zip -@ $TEX2RTFDIR/deliver/tex2rtf-source.zip < temp.txt
rm temp.txt

# Now make a tar archive
ls `cat $TEX2RTFDIR/distrib/src.rsp` > /tmp/tex2rtf.txt
tar cvf $TEX2RTFDIR/deliver/tex2rtf-source.tar -T /tmp/tex2rtf.txt
gzip $TEX2RTFDIR/deliver/tex2rtf-source.tar
rm /tmp/tex2rtf.txt


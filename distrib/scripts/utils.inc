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
      if [ "$line" != "" ]; then
        ls $line >> $outputfile
      fi
    uniq < $outputfile > /tmp/uniqtemp.txt
    mv /tmp/uniqtemp.txt $outputfile
    done
}
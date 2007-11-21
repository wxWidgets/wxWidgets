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

docopydocs()
{
    APPDIR=$1
    DESTDIR=$2
    
    cp $APPDIR/docs/readme.txt $DESTDIR/README.txt
    cp $APPDIR/docs/changes.txt $DESTDIR/CHANGES.txt
    cp $APPDIR/docs/licence.txt $DESTDIR/LICENCE.txt
    cp $APPDIR/docs/lgpl.txt $DESTDIR/COPYING.LIB
    
    cp $APPDIR/docs/changes.txt $DESTDIR/changes-$VERSION.txt
    cp $APPDIR/docs/readme.txt $DESTDIR/readme-$VERSION.txt

    cp $APPDIR/docs/msw/readme.txt $DESTDIR/readme-msw-$VERSION.txt
    cp $APPDIR/docs/msw/install.txt $DESTDIR/install-msw-$VERSION.txt

    cp $APPDIR/docs/mac/readme.txt $DESTDIR/readme-mac-$VERSION.txt
    cp $APPDIR/docs/mac/install.txt $DESTDIR/install-mac-$VERSION.txt

    cp $APPDIR/docs/motif/readme.txt $DESTDIR/readme-motif-$VERSION.txt
    cp $APPDIR/docs/motif/install.txt $DESTDIR/install-motif-$VERSION.txt

    cp $APPDIR/docs/gtk/readme.txt $DESTDIR/readme-gtk-$VERSION.txt
    cp $APPDIR/docs/gtk/install.txt $DESTDIR/install-gtk-$VERSION.txt

    cp $APPDIR/docs/x11/readme.txt $DESTDIR/readme-x11-$VERSION.txt
    cp $APPDIR/docs/x11/install.txt $DESTDIR/install-x11-$VERSION.txt

    cp $APPDIR/docs/mgl/readme.txt $DESTDIR/readme-mgl-$VERSION.txt
    cp $APPDIR/docs/mgl/install.txt $DESTDIR/install-mgl-$VERSION.txt
    
    cp $APPDIR/docs/dfb/install.txt $DESTDIR/install-dfb-$VERSION.txt

    cp $APPDIR/docs/cocoa/readme.txt $DESTDIR/readme-cocoa-$VERSION.txt
    cp $APPDIR/docs/cocoa/install.txt $DESTDIR/install-cocoa-$VERSION.txt

    cp $APPDIR/docs/base/readme.txt $DESTDIR/readme-base-$VERSION.txt

    cp $APPDIR/docs/os2/install.txt $DESTDIR/install-os2-$VERSION.txt

    cp $APPDIR/docs/univ/readme.txt $DESTDIR/readme-univ-$VERSION.txt

    cp $APPDIR/docs/readme_vms.txt $DESTDIR/readme-vms-$VERSION.txt

}

#!/bin/sh
# tardist: make up a tar.gz distribution of wxMicroWindows.

init=""
if [ "$1" = "" ]
then
  echo Usage: tardist wx-dir output-dir version
  exit
fi

if [ "$2" = "" ]
then
  echo Usage: tardist wx-dir output-dir version
  exit
fi

if [ "$3" = "" ]
then
  echo Usage: tardist wx-dir output-dir version
  exit
fi

WXVER=$3

echo About to archive wxWidgets:
echo   From   $1
echo   To     $2
echo CTRL-C if this is not correct.
read dummy

cd $1

echo Removing backup files...
rm *~ */*~ */*/*~ */*/*/*~ */*/*/*/*~

rm -f $2/wxMicroWindows-${WXVER}.*

echo Tarring...

ls `cat $1/distrib/msw/microwin.rsp` > /tmp/files.txt
tar cvf $2/wxMicroWindows-${WXVER}.tar -T /tmp/files.txt
gzip $2/wxMicroWindows-${WXVER}.tar
mv $2/wxMicroWindows-${WXVER}.tar.gz $2/wxMicroWindows-${WXVER}.tgz

echo Done!

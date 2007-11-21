#!/bin/sh
# Example script to call the maketarball.sh
# script, for creating the Tex2RTF distribution.
echo Make the Tex2RTF distribution

WXWIN=/opt/julians/wx2dev/wxWindows
VERSION=2.01
TEX2RTFBIN=$WXWIN/utils/tex2rtf/deliver/tex2rtf
DESTDIR=$WXWIN/utils/tex2rtf/deliver

$WXWIN/utils/tex2rtf/distrib/maketarball.sh $WXWIN $TEX2RTFBIN $DESTDIR $VERSION
sh zipsrc.sh $VERSION

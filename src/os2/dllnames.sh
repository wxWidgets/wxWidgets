#!/bin/sh
#
# dllnames - a tool to form short DLL names for wxWindows
#
# This script will accept an import library name and create
# a short(er) DLL name from it.

dllFile="$1"
case $dllFile in
*wx_base_*)
    dllFile=`echo $dllFile | sed 's/base_\(...\)/b\1/'`
    ;;
*wx_*_*)
    dllFile=`echo $dllFile | sed 's/_\(..\)[^_]*_\(..\)[^-]*-/\1\2/'`
    ;;
*)
    ;;
esac
dllFile="`echo $dllFile | sed 's/\.//' | sed 's/_//' | sed 's/-//'`"
echo $dllFile
exit 0
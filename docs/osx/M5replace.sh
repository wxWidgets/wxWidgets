#!/bin/sh
#-----------------------------------------------------------------------------
#-- Name:        docs/mac/M5replace.sh
#-- Purpose:     Replace a string in CodeWarrior exported XML project files
#-- Author:      Gilles Depeyrot
#-- Modified by:
#-- Created:     08.01.2002
#-- RCS-ID:      $Id$
#-- Copyright:   (c) 2001 Gilles Depeyrot
#-- Licence:     wxWindows licence
#-----------------------------------------------------------------------------

echo -n "Replace '$1' with '$2' in xml project files? [y/N]"
read ans

if [ "$ans" != "y" ] ; then
    exit
fi

echo "Searching for xml files..."
files=`find ../.. -name "*.xml" -print`

for f in $files
do
    cat $f | sed -e "s,$1,$2," > $f.new
    if [ "`diff -q $f $f.new`" != "" ] ; then
        mv $f.new $f
        echo "Replaced in $f..."
    else
        rm $f.new
    fi
done

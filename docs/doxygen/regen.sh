#!/bin/bash

# $Id$

# This bash script regenerates the HTML doxygen version of the
# wxWidgets manual and adjusts the doxygen log to make it more
# readable.

mkdir -p out/html       # we need to copy files in this folder below
mkdir -p out/html/wxmsw out/html/wxgtk out/html/wxmac

# this image is not automatically copied by Doxygen because it's not
# used in doxygen documentation but only in our html footer...
cp images/powered-by-wxwidgets.png out/html
cp images/*logo.png out/html
cp images/wxmsw/*png out/html/wxmsw
cp images/wxmac/*png out/html/wxmac
cp images/wxgtk/*png out/html/wxgtk

# this CSS is automatically copied by Doxygen because it's 
# included by our custom html header...
cp wxwidgets.css out/html

#
# NOW RUN DOXYGEN
#
# NB: we do this _after_ copying the required files to the output folders
#     otherwise when generating the CHM file with Doxygen, those files are
#     not included!
#
if [[ -z "$1" ]]; then
    cfgfile="Doxyfile.all"
else
    cfgfile="Doxyfile.$1"
fi

doxygen $cfgfile

# Doxygen has the annoying habit to put the full path of the
# affected files in the log file; remove it to make the log
# more readable
currpath=`pwd`/
interfacepath=`cd ../../interface && pwd`/
cat doxygen.log | sed -e "s|$currpath||g" -e "s|$interfacepath||g" >temp
mv temp doxygen.log

# filter out the following warning which we don't care about
#cat doxygen.log | grep -v ".*supplied.*as.*the.*argument.*is.*not.*an.*input.*file.*" >temp
#mv temp doxygen.log


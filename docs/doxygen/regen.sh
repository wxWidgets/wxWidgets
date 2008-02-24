#!/bin/sh

# $Id$

# This bash script regenerates the HTML doxygen version of the
# wxWidgets manual and adjusts the doxygen log to make it more
# readable.

doxygen

# this image is not automatically copied by Doxygen because it's not
# used in doxygen documentation but only in our html footer...
cp images/powered-by-wxwidgets.png out/html

# this CSS is automatically copied by Doxygen because it's 
# included by our custom html header...
cp wxwidgets.css out/html

# Doxygen has the annoying habit to put the full path of the
# affected files in the log file; remove it to make the log
# more readable
currpath=`pwd`/
interfacepath=`cd ../../interface && pwd`/
cat doxygen.log | sed -e "s|$currpath||g" -e "s|$interfacepath||g" >temp
mv temp doxygen.log

# filter out the following warning which we don't care about
cat doxygen.log | grep -v ".*supplied.*as.*the.*argument.*is.*not.*an.*input.*file.*" >temp
mv temp doxygen.log


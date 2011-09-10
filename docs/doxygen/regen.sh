#!/bin/bash
#
# $Id$
#
# This bash script regenerates the HTML doxygen version of the
# wxWidgets manual and adjusts the doxygen log to make it more
# readable.
#
# Usage:
#    ./regen.sh [html|chm|xml|latex|all]
#
# Pass "x" to regen only the X output format and "all" to regen them all.
# If no arguments are passed all formats are regenerated
# (just like passing "all").
#


# remember current folder and then cd to the docs/doxygen one
me=$(basename $0)
path=${0%%/$me}        # path from which the script has been launched
current=$(pwd)
cd $path
export WXWIDGETS=`cd ../.. && pwd`

# prepare folders for the cp commands below
mkdir -p out/html       # we need to copy files in this folder below
mkdir -p out/html/generic out/html/wxmsw out/html/wxgtk out/html/wxmac

# These are not automatically copied by Doxygen because they're not
# used in doxygen documentation, only in our html footer and by our
# custom aliases
cp images/powered-by-wxwidgets.png out/html
cp images/logo_*.png out/html
cp images/tab_*.gif out/html
cp images/wxmsw/*png out/html/wxmsw
cp images/wxmac/*png out/html/wxmac
cp images/wxgtk/*png out/html/wxgtk
cp images/generic/*png out/html/generic
cp wxwidgets.js out/html

# these CSS are not automatically copied by Doxygen because they're
# included by our custom html header...
cp wxwidgets.css out/html
cp wxtabs.css out/html

# which configuration should we use?
if [[ -z "$1" ]]; then
    cfgfile="Doxyfile_all"
else
    cfgfile="Doxyfile_$1"
fi

#
# NOW RUN DOXYGEN
#
# NB: we do this _after_ copying the required files to the output folders
#     otherwise when generating the CHM file with Doxygen, those files are
#     not included!
#
doxygen $cfgfile

if [[ "$1" = "qch" ]]; then
    # we need to add missing files to the .qhp
    cd out/html
    qhelpfile="index.qhp"

    # remove all <file> and <files> tags
    cat $qhelpfile | grep -v "<file" >temp

    # remove last 4 lines (so we have nothing after the last <keyword> tag)
    lines=$(wc -l < temp)
    wanted=`expr $lines - 4`
    head -n $wanted temp >$qhelpfile

    # generate a list of new <keyword> tags to add to the index file; without
    # this step in the 'index' tab of Qt assistant the "wxWindow" class is not
    # present; just "wxWindow::wxWindow" ctor is listed.
    # NOTE: this operation is not indispensable but produces a QCH easier to use IMO.
    sed -e 's/<keyword name="wx[a-zA-Z~]*" id="wx\([a-zA-Z]*\)::[a-zA-Z~]*" ref="\([a-z_]*.html\)#.*"/<keyword name="wx\1" id="wx\1" ref="\2"/g' < $qhelpfile | grep "<keyword name=\"wx" | uniq >temp
    cat temp >>$qhelpfile
    echo "    </keywords>" >>$qhelpfile
    echo "    <files>" >>$qhelpfile

    # remove useless files to make the qch slim
    rm temp *map *md5

    # add a <file> tag for _any_ file in out/html folder except the .qhp itself
    for f in * */*png; do
        if [[ $f != $qhelpfile ]]; then
            echo "      <file>$f</file>" >>$qhelpfile
        fi
    done

    # add ending tags to the qhp file
    echo "    </files>
  </filterSection>
</QtHelpProject>" >>$qhelpfile

    # replace keyword names so that they appear fully-qualified in the
    # "index" tab of the Qt Assistant; e.g. Fit => wxWindow::Fit
    # NOTE: this operation is not indispendable but produces a QCH easier to use IMO.
    sed -e 's/<keyword name="[a-zA-Z:~]*" id="\([a-zA-Z]*::[a-zA-Z~]*\)"/<keyword name="\1" id="\1"/g' <$qhelpfile >temp
    mv temp $qhelpfile

    # last, run qhelpgenerator:
    cd ../..
    qhelpgenerator out/html/index.qhp -o out/wx.qch
fi

if [[ "$1" = "docset" ]]; then
    DOCSETNAME="org.wxwidgets.doxygen.wx29.docset"
    ATOM="org.wxwidgets.doxygen.docset.wx29.atom"
    ATOMDIR="http://docs.wxwidgets.org/docsets"
    XAR="org.wxwidgets.doxygen.docset.wx29.xar"
    XARDIR="http://docs.wxwidgets.org/docsets"
    XCODE_INSTALL=`sh xcode-select -print-path`
    
    cp wxdocsettabs.css out/html/wxtabs.css
    cp wxdocsetwidgets.css out/html/wxwidgets.css
    cp img_downArrow.png out/html
    cp background_navigation.png out/html
    
    cd out/html
    DESTINATIONDIR=`pwd`
    
    rm -rf $DESTINATIONDIR/$DOCSETNAME
    rm -f $DESTINATIONDIR/$XAR
    
    make 
    
    defaults write $DESTINATIONDIR/$DOCSETNAME/Contents/Info CFBundleVersion 1.3
    defaults write $DESTINATIONDIR/$DOCSETNAME/Contents/Info CFBundleShortVersionString 1.3
    defaults write $DESTINATIONDIR/$DOCSETNAME/Contents/Info CFBundleName "wxWidgets 2.9 Library"
    defaults write $DESTINATIONDIR/$DOCSETNAME/Contents/Info DocSetFeedURL $ATOMDIR/$ATOM
    defaults write $DESTINATIONDIR/$DOCSETNAME/Contents/Info DocSetFallbackURL http://docs.wxwidgets.org
    defaults write $DESTINATIONDIR/$DOCSETNAME/Contents/Info DocSetDescription "API reference and conceptual documentation for wxWidgets 2.9"
    defaults write $DESTINATIONDIR/$DOCSETNAME/Contents/Info NSHumanReadableCopyright "Copyright 1992-2011 wxWidgets team, Portions 1996 Artificial Intelligence Applications Institute"
    
    $XCODE_INSTALL/usr/bin/docsetutil package -atom $DESTINATIONDIR/$ATOM -download-url $XARDIR/$XAR -output $DESTINATIONDIR/$XAR $DESTINATIONDIR/$DOCSETNAME

    cd ../..
fi

# Doxygen has the annoying habit to put the full path of the
# affected files in the log file; remove it to make the log
# more readable
currpath=`pwd`/
interfacepath=`cd ../../interface && pwd`/
cat doxygen.log | sed -e "s|$currpath||g" -e "s|$interfacepath||g" >temp

# Doxygen warnings are not completely sorted for filename; enforce correct sorting:
cat temp | sort >doxygen.log
rm temp

# return to the original folder from which this script was launched
cd $current

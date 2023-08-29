#!/bin/bash
#
#
# This bash script regenerates the HTML doxygen version of the
# wxWidgets manual and adjusts the doxygen log to make it more
# readable.
#
# Usage:
#    ./regen.sh [html|chm|xml|latex|docset|all]
#
# Pass "x" to regen only the X output format and "all" to regen them all.
# If no arguments are passed, HTML is regenerated (just like passing "html").
#


# cd to the directory this script is in
me=$(basename $0)
path=${0%%/$me}        # path from which the script has been launched
cd "$path"
SCRIPTS_DIR="$(pwd)/scripts"

if [[ -z "$WXWIDGETS" ]]; then
    # Notice the use of -P to ensure we get the canonical path even if there
    # are symlinks in the current path. This is important because Doxygen
    # strips this string from the paths in the generated files textually and it
    # wouldn't work if it contained symlinks.
    WXWIDGETS=`cd ../.. && pwd -P`
    if [ "$OSTYPE" = "cygwin" ]; then
        WXWIDGETS=`cygpath -w $WXWIDGETS`
    fi
    export WXWIDGETS
fi

if [ "$DOXYGEN" = "" ]; then
    DOXYGEN=doxygen
fi

# Check that doxygen has the correct version as different versions of it are
# unfortunately not always (in fact, practically never) compatible.
#
# Still allow using incompatible version if explicitly requested.
if [[ -z $WX_SKIP_DOXYGEN_VERSION_CHECK ]]; then
    doxygen_version=`$DOXYGEN --version`
    doxygen_version_required=1.9.1
    if [[ $doxygen_version != $doxygen_version_required ]]; then
        echo "Doxygen version $doxygen_version is not officially supported."
        echo "Please use Doxygen $doxygen_version_required or export WX_SKIP_DOXYGEN_VERSION_CHECK."
        exit 1
    fi
fi

if [[ -z $WX_HTML_OUTPUT_DIR ]]; then
    WX_HTML_OUTPUT_DIR=out/html
fi

# prepare folders for the cp commands below
mkdir -p $WX_HTML_OUTPUT_DIR       # we need to copy files in this folder below
mkdir -p $WX_HTML_OUTPUT_DIR/generic

# These are not automatically copied by Doxygen because they're not
# used in doxygen documentation, only in our html footer and by our
# custom aliases
cp images/generic/*png $WX_HTML_OUTPUT_DIR/generic

# These values are always used here, the corresponding options only exist in
# order to allow not using them when generating CHM in regen.bat.
export DOT_IMAGE_FORMAT='svg'
export HTML_HEADER='custom_header.html'
export CUSTOM_THEME_CSS='doxygen-awesome-css/doxygen-awesome.css'
export CUSTOM_THEME_JS1='doxygen-awesome-css/doxygen-awesome-darkmode-toggle.js'
export CUSTOM_THEME_JS2='doxygen-awesome-css/doxygen-awesome-fragment-copy-button.js'

# Defaults for settings controlled by this script
export GENERATE_DOCSET="NO"
export GENERATE_HTML="NO"
export GENERATE_HTMLHELP="NO"
export GENERATE_LATEX="NO"
export GENERATE_QHP="NO"
export GENERATE_XML="NO"
export SEARCHENGINE="NO"
export SERVER_BASED_SEARCH="NO"

# Which format should we generate during this run?
case "$1" in
    all) # All *main* formats, not all formats, here for backwards compat.
        export GENERATE_HTML="YES"
        export GENERATE_HTMLHELP="YES"
        export GENERATE_XML="YES"
        ;;
    chm)
        export GENERATE_HTML="YES"
        export GENERATE_HTMLHELP="YES"
        ;;
    docset)
        export GENERATE_DOCSET="YES"
        export GENERATE_HTML="YES"
        export GENERATE_TAGFILE="$path/out/wxWidgets.tag"
        ;;
    latex)
        export GENERATE_LATEX="YES"
        ;;
    php) # HTML, but with PHP Search Engine
        export GENERATE_HTML="YES"
        export SEARCHENGINE="YES"
        export SERVER_BASED_SEARCH="YES"
        ;;
    qch)
        export GENERATE_HTML="YES"
        export GENERATE_QHP="YES"
        ;;
    xml)
        export GENERATE_XML="YES"
        ;;
    *) # Default to HTML only
        export GENERATE_HTML="YES"
        export SEARCHENGINE="YES"
        ;;
esac

#
# NOW RUN DOXYGEN
#
# NB: we do this _after_ copying the required files to the output folders
#     otherwise when generating the CHM file with Doxygen, those files are
#     not included!
#
$DOXYGEN Doxyfile

if [[ "$1" = "php" ]]; then
    # Work around a bug in Doxygen < 1.8.19 PHP search function.
    cp custom_search_functions.php $WX_HTML_OUTPUT_DIR/search_functions.php
fi

if [[ "$1" = "qch" ]]; then
    # we need to add missing files to the .qhp
    cd $WX_HTML_OUTPUT_DIR
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

    # add a <file> tag for _any_ file in this directory except the .qhp itself
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
    qhelpgenerator $WX_HTML_OUTPUT_DIR/index.qhp -o out/wx.qch
fi

if [[ "$1" = "docset" ]]; then
    BASENAME="wxWidgets-3.1"    # was org.wxwidgets.doxygen.docset.wx30
    DOCSETNAME="$BASENAME.docset"
    ATOM="$BASENAME.atom"
    ATOMDIR="https://docs.wxwidgets.org/docsets"
    XAR="$BASENAME.xar"
    XARDIR="https://docs.wxwidgets.org/docsets"

    # See if xcode is installed
    if [ -x "$(command -v xcode-select)" ]; then
        XCODE_INSTALL=`xcode-select -print-path`
    fi

    cd $WX_HTML_OUTPUT_DIR
    DESTINATIONDIR=`pwd`/../docset

    mkdir -p $DESTINATIONDIR
    rm -rf $DESTINATIONDIR/$DOCSETNAME
    rm -f $DESTINATIONDIR/$XAR

    make DOCSET_NAME=$DESTINATIONDIR/$DOCSETNAME

    # Choose which plist modification utility to use
    if [ -x "$(command -v defaults)" ]; then
        PLIST_WRITE_CMD="defaults write"
    else
        PLIST_WRITE_CMD="python $SCRIPTS_DIR/write_info_tag.py"
    fi

    # Modify the Info.plist file
    $PLIST_WRITE_CMD $DESTINATIONDIR/$DOCSETNAME/Contents/Info CFBundleVersion 1.3
    $PLIST_WRITE_CMD $DESTINATIONDIR/$DOCSETNAME/Contents/Info CFBundleShortVersionString 1.3
    $PLIST_WRITE_CMD $DESTINATIONDIR/$DOCSETNAME/Contents/Info CFBundleName "wxWidgets 3.1"
    $PLIST_WRITE_CMD $DESTINATIONDIR/$DOCSETNAME/Contents/Info DocSetFeedURL $ATOMDIR/$ATOM
    $PLIST_WRITE_CMD $DESTINATIONDIR/$DOCSETNAME/Contents/Info DocSetFallbackURL https://docs.wxwidgets.org
    $PLIST_WRITE_CMD $DESTINATIONDIR/$DOCSETNAME/Contents/Info DocSetDescription "API reference and conceptual documentation for wxWidgets 3.0"
    $PLIST_WRITE_CMD $DESTINATIONDIR/$DOCSETNAME/Contents/Info NSHumanReadableCopyright "Copyright 1992-2023 wxWidgets team, Portions 1996 Artificial Intelligence Applications Institute"
    $PLIST_WRITE_CMD $DESTINATIONDIR/$DOCSETNAME/Contents/Info isJavaScriptEnabled true
    $PLIST_WRITE_CMD $DESTINATIONDIR/$DOCSETNAME/Contents/Info dashIndexFilePath index.html
    $PLIST_WRITE_CMD $DESTINATIONDIR/$DOCSETNAME/Contents/Info DocSetPlatformFamily wx

    echo "Creating docset database"
    if ! [ -z "$XCODE_INSTALL" ]; then
        # Use xcode to create the docset if it is installed
        $XCODE_INSTALL/usr/bin/docsetutil package -atom $DESTINATIONDIR/$ATOM -download-url $XARDIR/$XAR -output $DESTINATIONDIR/$XAR $DESTINATIONDIR/$DOCSETNAME
    else
        # Use doxytag2zealdb to create the database
        # This requires the python package doxytag2zealdb installed
        python -m doxytag2zealdb --tag $DESTINATIONDIR/../wxWidgets.tag --db $DESTINATIONDIR/$DOCSETNAME/Contents/Resources/docSet.dsidx --include-parent-scopes --include-function-signatures
    fi

    # Copy the icon
    cp $SCRIPTS_DIR/../../../art/wxwin16x16.png $DESTINATIONDIR/$DOCSETNAME/icon.png
    cp $SCRIPTS_DIR/../../../art/wxwin32x32.png $DESTINATIONDIR/$DOCSETNAME/icon@2x.png
    cd ../..
fi

# Doxygen has the annoying habit to put the full path of the
# affected files in the log file; remove it to make the log
# more readable
if [[ -s doxygen.log ]]; then
    topsrcdir=`cd ../.. && pwd`
    sed -i'' -e "s|$topsrcdir/||g" doxygen.log

    echo '*** There were warnings during docs generation ***'
else
    # Don't leave empty file lying around.
    rm doxygen.log
fi

#!/bin/sh
#############################################################################
# Name:        validate
# Purpose:     Reports errors in wxWidgets buildbot configuration files
# Author:      Mike Wetherell
# RCS-ID:      $Id$
# Copyright:   (c) 2007 Mike Wetherell
# Licence:     wxWidgets licence
#############################################################################

if [ $# -eq 0 -o ! -f "$1" ]; then
    echo "Usage: $0 FILE..."
    echo "Reports errors in wxWidgets buildbot configuration files"
    exit 1
fi >&2

DIR="`dirname $0`"
WORKDIR="${TMPDIR:-/tmp}/wx.$$"
mkdir "$WORKDIR" || exit
trap 'rm -rf "$WORKDIR"' EXIT
WORKPAT=`echo "$WORKDIR" | sed 's|[^A-Za-z0-9/]|.|g'`

# Change the names of the temporary files in an error message to something
# to something more informative
#
error()
{
    if [ -n "$1" ]; then
        echo "$1" |
            sed "s|file ${WORKPAT}|${WORKPAT}|g;\
                 s|${WORKPAT}/XSLT|generated XSLT (from $NAME)|g;\
                 s|${WORKPAT}/prep|$NAME (preprocessed)|g"
    fi
}

# This is pretty ugly, sorry. It tries not to print the same error more than
# once, and it tries to send success message to stdout and errors to stderr.
# It still doesn't return a meaningful exit code.
#
while [ $# -gt 0 ]
do
    INPUT="$1"
    NAME="`echo \"$INPUT\" | sed 's/[|\]/\\\&/g'`"
    XSLT="$WORKDIR/XSLT"
    OUTPUT="$WORKDIR/prep"

    if STDERR=`xsltproc --xinclude -o "$XSLT" $DIR/embedded.xsl "$INPUT" 2>&1`
    then
        STDERR=`xsltproc --xinclude -o "$OUTPUT" "$XSLT" "$INPUT" 2>&1` \
            && OK=true || OK=false
        error "$STDERR" >&2

        if $OK; then
            STDERR=`xmllint --noout --schema $DIR/bot.xsd "$OUTPUT" 2>&1` \
                && OUT=1 || OUT=2
            error "$STDERR" >&$OUT
        fi
    else
        error "$STDERR" >&2
    fi

    rm -f "$XSLT" "$OUTPUT"

    shift
done

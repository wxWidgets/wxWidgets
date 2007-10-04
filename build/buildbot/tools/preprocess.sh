#!/bin/sh
#############################################################################
# Name:        preprocess
# Purpose:     Expand embedded xslt
# Author:      Mike Wetherell
# RCS-ID:      $Id$
# Copyright:   (c) 2007 Mike Wetherell
# Licence:     wxWidgets licence
#############################################################################

if [ $# -eq 0 -o ! -f "$1" ]; then
    echo "Usage: $0 FILE..."
    echo "Expands embedded xslt"
    exit 1
fi >&2

DIR="`dirname $0`"

while [ $# -gt 0 ]; do
    xsltproc --xinclude $DIR/embedded.xsl "$1" 2>/dev/null |
        xsltproc --xinclude - "$1"
    shift
done

#!/bin/sh

###############################################################################
# Name:      is_text.sh
# Purpose:   returns 0 if the file in cvs tree is text, 1 if binary
# Version:   $Id$
# Author:    VZ
# Created:   2006-07-19
# Copyright: (c) Vadim Zeitlin 2006 <vadim@wxwindows.org>
###############################################################################

if [ $# != 1 ]; then
    echo "Usage: $0 <file>" >&2
    exit 2
fi

# don't do configure; config.sub; config.guess
if [ `basename $1` = "configure" ] ; then
    exit 1
fi
if [ `basename $1` = "config.sub" ] ; then
    exit 1
fi
if [ `basename $1` = "config.guess" ] ; then
    exit 1
fi

entries=`dirname $1`/CVS/Entries
if [ ! -f $entries ]; then
    echo "CVS entries file \"$entries\" not found." >&2
    exit 3
fi

# we look for -kb in the last field, optionally followed by tag name
re="^/`basename $1`/.*/-kb/\(T[^/]*\)\{0,1\}$"
if grep -q "$re" $entries; then
    exit 1
fi

exit 0

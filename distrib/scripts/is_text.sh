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

# ignore makefile.wat etc
if [ `basename $1` = "makefile.wat" ] ; then
    exit 1
fi
if [ `basename $1` = "config.wat" ] ; then
    exit 1
fi
	
if  svn proplist $1 | grep -q "eol-style"  ; then
    exit 0
fi    

#either not a svn file or not one with eol-style
exit 3

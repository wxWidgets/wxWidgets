#!/bin/sh
if [ $# != 1 ]; then
    echo "Usage: $0 <file>" >&2
    exit 2
fi

entries=`dirname $1`/CVS/Entries
if [ ! -f $entries ]; then
    echo "CVS entries file \"$entries\" not found." >&2
    exit 3
fi

re="^/`basename $1`/.*/-kb/\$"
if grep -q "$re" $entries; then
    exit 1
fi

exit 0
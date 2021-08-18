##############################################################################
# Name:       misc/scripts/run_sed_common.sh
# Purpose:    Common helpers for scripts using sed for automatic updates
# Created:    2019-04-21 (extracted from misc/scripts/inc_release)
# Copyright:  (c) 2007,2019 Vadim Zeitlin <vadim@wxwidgets.org>
# Licence:    wxWindows licence
##############################################################################

error() {
    echo "$progname: $*" >&2
}

msg() {
    echo "$progname: $*"
}

msgn() {
    echo -n "$progname: $*"
}

msgc() {
    echo "$*"
}

run_sed() {
    filename=$1
    shift
    cmd=
    while [ $# -gt 0 ]; do
        cmd="$cmd-e \"$1\" "
        shift
    done

    msgn "	processing $filename ...	"
    eval "sed $cmd $filename" > $filename.$$
    if cmp -s $filename $filename.$$; then
        rm $filename.$$
        msgc "unchanged"
    else
        chmod --reference=$filename $filename.$$
        mv $filename.$$ $filename
        msgc "done"
    fi
}

progname=`basename $0`

# we must be run from wx directory
if [ ! -f wxwin.m4 ]; then
    error "must be ran from root wx directory"
    exit 1
fi

# exit on any error
set -e

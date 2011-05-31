#!/bin/sh
#############################################################################
# Name:        check.sh
# Purpose:     Offline checker for the buildbot configuration files
# Author:      Mike Wetherell
# RCS-ID:      $Id$
# Copyright:   (c) 2007 Mike Wetherell
# Licence:     wxWindows licence
#############################################################################

usage() {
    echo "Usage: $0 [options] FILE..."
    echo "Offline checker for the buildbot configuration files"
    echo
    echo "Options:"
    echo "  -g      generate xslt"
    echo "  -h      show help"
    echo "  -l N    output only line N"
    echo "  -p      preprocess"
    echo "  -v      validate"
    exit 0
}

badopt() {
    echo "try '$0 -h' for more information" >&2
    exit 1
}

progcheck() {
    prog="$1"
    $prog --version >/dev/null 2>&1 || {
        echo "$0: requires $prog, not found" >&2
        exit 1
    }
}

GENERATE=1
PREPROCESS=2
VALIDATE=3

MODE=$VALIDATE
FILTER=cat

while getopts ghl:pv opt; do
    case "$opt" in
        \?) badopt ;;
        g)  MODE=$GENERATE ;;
        h)  usage ;;
        l)  FILTER="sed -ne ${OPTARG}p" ;;
        p)  MODE=$PREPROCESS ;;
        v)  MODE=$VALIDATE ;;
    esac
done

if [ $OPTIND -gt 1 ]; then
    shift `expr $OPTIND - 1`
fi

if [ $# -eq 0 ]; then
    usage
fi

XSLTPROC=xsltproc
XMLLINT=xmllint

progcheck $XSLTPROC
progcheck $XMLLINT

DIR="`dirname $0`"
WORKDIR="${TMPDIR:-/tmp}/wx.$$"
mkdir "$WORKDIR" || exit
trap 'rm -rf "$WORKDIR"' EXIT
WORKPAT=`echo "$WORKDIR" | sed 's|[^A-Za-z0-9]/|.|g'`
XSLT="$WORKDIR/XSLT"
PREP="$WORKDIR/PREP"
STDERR="$WORKDIR/STDERR"
ERR=0

# Filter to show lines of genertated XSLT when they are mentioned.
showxslt() {
    awk '{
            print;
            if (sub(/.*generated XSLT line */, "") && sub(/[^0-9].*/, ""))
            {
                system("sed -ne "$0"p '$XSLT'");
            }
         }'
}

# Test it works as old version of awk don't have sub or system functions.
if showxslt </dev/null 2>/dev/null; then
    SHOWXSLT=showxslt
else
    SHOWXSLT=cat
fi

# Change the names of the temporary files in an error message to something
# to something more informative
errout()
{
    NAME="$1"

    if [ -s "$STDERR" ]; then
        sed "s|file ${WORKPAT}|${WORKPAT}|g;\
             s|${WORKPAT}/XSLT|generated XSLT|g;\
             s|${WORKPAT}/PREP|$NAME (preprocessed)|g" "$STDERR" | $SHOWXSLT
    fi
}

output()
{
    $FILTER "$1"
}

generate()
{
    INPUT="$1"

    if $XSLTPROC --xinclude -o "$XSLT" $DIR/embedded.xsl "$INPUT" 2>"$STDERR" &&
       test \! -s "$STDERR"
    then
        if [ $MODE -eq $GENERATE ]; then
            output "$XSLT"
        fi
    else
        return 1
    fi
}

preprocess()
{
    INPUT="$1"

    if [ $MODE -lt $PREPROCESS ]; then
        return 0
    fi

    if $XSLTPROC --xinclude -o "$PREP" "$XSLT" "$INPUT" 2>"$STDERR" &&
       test \! -s "$STDERR"
    then
        if [ $MODE -eq $PREPROCESS ]; then
            output "$PREP"
        fi
    else
        return 1
    fi
}

validate()
{
    NAME="$1"

    if [ $MODE -lt $VALIDATE ]; then
        return 0
    fi

    if $XMLLINT --noout --schema $DIR/bot.xsd "$PREP" 2>"$STDERR"
    then
        errout "$NAME"
    else
        return 1
    fi
}

while [ $# -gt 0 ]; do
    INPUT="$1"
    NAME="`echo \"$INPUT\" | sed 's/[|\]/\\\&/g'`"

    {
        generate "$INPUT" &&
        preprocess "$INPUT" &&
        validate "$NAME"
    } || {
        errout "$NAME" >&2
        ERR=1
    }

    rm -f "$XSLT" "$PREP" "$STDERR"

    shift
done

exit $ERR

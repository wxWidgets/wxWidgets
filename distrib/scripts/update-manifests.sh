#!/bin/sh
##############################################################################
# Name:       distrib/scripts/update-manifests.sh
# Purpose:    Run from this directory to update some of the manifests/*.rsp
#             files that can be generated automatically (vc, dmc, makefile,
#             wince).
#
#             Note that it's best to run this after running bakefile_gen,
#             otherwise this script can be quite slow.
# Created:    2006-11-13
# RCS-ID:     $Id$
# Copyright:  (c) 2006 Vaclav Slavik <vaclav@wxwindows.org>
# Licence:    wxWindows licence
##############################################################################

MANIFESTS_DIR=$(pwd)/manifests
WX_ROOT=$(cd ../.. ; pwd)

cd $WX_ROOT

# generates manifest from bakefile output files:
generate_manifest() 
{
    manifest_file="${MANIFESTS_DIR}/$1"
    formats="$2"
    shift
    shift

    # generate files from given format:
    (
    cd ${WX_ROOT}/build/bakefiles
    bakefile_gen -d ../../distrib/scripts/Bakefiles.release.bkgen \
                 --format $formats --list-files | \
        sed -e "s@^${WX_ROOT}/@@" | sort | uniq \
        >$manifest_file
    )

    # and add manually added files to the list:
    for f in $* ; do
        echo $f >>$manifest_file
    done
}

# generates manifest from all files present in CVS under given directories:
do_list_all_cvs_files()
{
    local manifest manifest_file files subdirs d f ff s ss

    manifest="$1"
    manifest_file="${MANIFESTS_DIR}/${manifest}"
    shift

    for d in $* ; do
        # list all files:
        files=$(cat $d/CVS/Entries | sed -ne 's@^/\([^/]*\)/.*@\1@p')
        for f in $files ; do
            ff="$d/$f"
            if [ -f $ff ] ; then
                echo "$ff" >>$manifest_file
            fi
        done

        # recurse into subdirs:
        subdirs=$(cat $d/CVS/Entries | sed -ne 's@^D/\([^/]*\)/.*@\1@p')
        for s in $subdirs ; do
            ss="$d/$s"
            if [ -d $ss ] ; then
                do_list_all_cvs_files "$manifest" "$ss"
            fi
        done
    done
}

list_all_cvs_files()
{
    rm -f "${MANIFESTS_DIR}/$1"
    do_list_all_cvs_files $*
}


list_all_cvs_files generic_samples.rsp \
                   samples


generate_manifest dmc.rsp \
                  dmars,dmars_smake

generate_manifest vc.rsp \
                  msvc6prj \
                  "include/msvc/wx/setup.h" \
                  `(cd $WX_ROOT/src ; ls */*.ds?)`

generate_manifest makefile.rsp \
                  autoconf \
                  "locale/Makefile" \
                  "samples/Makefile.in" \
                  "libs/vms.opt" \
                  "libs/vms_gtk.opt" \
                  `find . -name descrip.mms`

generate_manifest wince.rsp \
                  msevc4prj \
                  "build/wince/missing"

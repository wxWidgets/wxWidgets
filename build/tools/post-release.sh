#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

topdir=`dirname $0`/../..

# Build the file list for sha1sums, from `docs/release.md`
declare -a files=(`sed -n '/^## Download Verification/,/^## Binaries/p' $topdir/docs/release.md | sed -n -E 's/^\s*0{40}\s{2}(wx.*)/\1/p'`)

# Get the release version
ver_string=`grep '#define wxVERSION_STRING ' $topdir/include/wx/version.h | sed 's/^.*"wxWidgets \(.*\)")/\1/'`

for i in "${files[@]}"
do
    # compute sha1sum
    sha1sum=`sha1sum $topdir/distrib/release/$ver_string/$i | cut -d' ' -f1`

    # save the sha1sum for this file
    sed -i -E "/^\s*[0]{40}\s{2}wx/ s/(^\s*)[0]{40}(\s{2}$i)/\1$sha1sum\2/" $topdir/docs/release.md
done

# Commit sha1sum related changes
git commit -m "Update released files sha1sums after $ver_string release" $topdir/docs/release.md

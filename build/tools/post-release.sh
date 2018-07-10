#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

topdir=`dirname $0`/../..

# Build the file list for sha1sums, from `docs/release.md`
declare -a files=(`sed -n '/^## Download Verification/,/^## Binaries/p' $topdir/docs/release.md | sed -n -E 's/^\s*0{40}\s{2}(wx.*)/\1/p'`)

# Get the release version unless it's given explicitly on the command line.
if [ -z $1 ]; then
    ver_string=`grep '#define wxVERSION_STRING ' $topdir/include/wx/version.h | sed 's/^.*"wxWidgets \(.*\)")/\1/'`
else
    ver_string=$1
fi

for i in "${files[@]}"
do
    # compute sha1sum
    sha1sum=`sha1sum $topdir/distrib/release/$ver_string/$i | cut -d' ' -f1`

    # save the sha1sum for this file
    sed -i -E "/^\s*[0]{40}\s{2}wx/ s/(^\s*)[0]{40}(\s{2}$i)/\1$sha1sum\2/" $topdir/docs/release.md
done

echo "File $topdir/docs/release.md updated after $ver_string release, don't forget to commit it."

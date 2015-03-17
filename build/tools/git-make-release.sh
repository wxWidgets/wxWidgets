#!/bin/sh
#
# This is the official script used to make wxWidgets releases.
#
# We use the git export features to track which files should be included in the
# distribution and we don't need to maintain the list of them ourselves but we
# also don't run the risk of including anything unwanted.
#
# To summarize, here are the steps to create the release:
#
#   % git svn tag WX_x_y_z
#   % ./build/tools/git-make-release.sh x.y.z
#   % ... upload ../wxWidgets-x.y.z.{7z,tar.bz2,zip} ...

version=$1
if [ -z "$version" ]; then
    echo "Must specify the distribution version." >&2
    exit 1
fi

if ! git diff --quiet; then
    echo "Working copy has modifications, commit or stash them." >&2
    exit 2
fi

set -e
set -x

prefix=wxWidgets-$version
destdir=$(dirname $(readlink -f $0))/../../../$prefix

cleanup() {
    rm -rf $destdir
}

trap cleanup INT TERM EXIT

cleanup

git archive --prefix=$prefix/ HEAD | (cd ..; tar x)
cd ..
# All setup0.h files are supposed to be renamed to just setup.h when checked
# out and in the distribution.
find $prefix/include/wx -type f -name setup0.h | while read f; do
    mv $f ${f%0.h}.h
done

# Compile gettext catalogs.
make -C $prefix/locale allmo

tar cjf $prefix.tar.bz2 $prefix

cd $prefix

zip -q -r ../$prefix.zip .

7z a ../$prefix.7z . >/dev/null
7z a ../${prefix}_headers.7z include >/dev/null

#!/bin/sh
#
# This is the official script used to make wxWidgets releases.
#
# We use the git export features to track which files should be included in the
# distribution and we don't need to maintain the list of them ourselves but we
# also don't run the risk of including anything unwanted.
#
# See docs/contributing/how-to-release.md for usage instructions.

version=$1
if [ -z "$version" ]; then
    echo "Must specify the distribution version." >&2
    exit 1
fi

root="$(readlink -f $(dirname $(readlink -f $0))/../..)"
cd "$root"

if ! git diff --quiet; then
    echo "Working copy has modifications, commit or stash them." >&2
    exit 2
fi

set -e
set -x

prefix=wxWidgets-$version
destdir="$root/distrib/release/$version"

cleanup() {
    rm -rf $destdir/$prefix
}

trap cleanup INT TERM EXIT

cleanup

mkdir -p $destdir

# We use GNU tar -i option to allow successfully extracting files from several
# tar archives concatenated together, without it we'd have to pipe output of
# each git-archive separately.
(git archive --prefix=$prefix/ HEAD;
 git submodule foreach --quiet "cd $root/\$path && git archive --prefix=$prefix/\$path/ HEAD") |
tar x -C $destdir -i

cd $destdir

# Compile gettext catalogs.
make -C $prefix/locale -s MSGFMT=msgfmt allmo

tar cjf $prefix.tar.bz2 $prefix

cd $prefix

zip -q -r ../$prefix.zip .

7z a ../$prefix.7z . >/dev/null
7z a ../${prefix}-headers.7z include >/dev/null

# Build HTML documentation packages.
prefix_docs=$prefix-docs-html
cd "$root/docs/doxygen"
rm -rf out
./regen.sh html
cd out
mv html "$prefix_docs"
tar cjf "$destdir/$prefix_docs.tar.bz2" "$prefix_docs"
cd "$prefix_docs"
zip -q -r "$destdir/$prefix_docs.zip" .
cd "$root"
rm -rf "$root/docs/doxygen/out"

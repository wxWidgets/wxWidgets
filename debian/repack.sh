#!/bin/sh
# Repackage upstream source to exclude non-distributable files
# should be called as "debian/repack.sh --upstream-source <ver> <downloaded file>
# (for example, via uscan)

set -e
set -u

VER="$2"
FILE="$3"
PKG=`dpkg-parsechangelog|sed 's/^Source: //p;d'`
LOCALVER=`dpkg-parsechangelog|sed 's/^Version: \(.*\)-.*/\1/p;d'`

REPACK_DIR="$PKG-$VER.orig" # DevRef § 6.7.8.2

echo -e "\nRepackaging $FILE\n"

DIR=`mktemp -d ./tmpRepackXXXXXX`
trap "rm -rf \"$DIR\"" QUIT INT EXIT

# Create an extra directory to cope with rootless tarballs
UP_BASE="$DIR/unpack"
mkdir "$UP_BASE"
tar xf "$FILE" -C "$UP_BASE"

if [ `ls -1 "$UP_BASE" | wc -l` -eq 1 ]; then
	# Tarball does contain a root directory
	UP_BASE="$UP_BASE/`ls -1 "$UP_BASE"`"
fi

## Remove stuff

# Lacking (suitable) source code.
find "$UP_BASE" -iname '*.swf' -delete

# Seemingly only rebuildable with non-free tools.
rm -f "$UP_BASE"/samples/help/doc.chm

# Contains Java class files without source.
rm -rf "$UP_BASE"/samples/help/doc

# We don't use these bundled libraries, so since we're repacking for other
# reasons we might as well delete them.  This avoids lintian warning about
# irrelevant issues in them, and ensures that we don't accidentally start
# building against them in future.
rm -rf "$UP_BASE"/src/expat
rm -rf "$UP_BASE"/src/jpeg
rm -rf "$UP_BASE"/src/png
rm -rf "$UP_BASE"/src/tiff
rm -rf "$UP_BASE"/src/zlib

# Now rebuild the tarball.
mv "$UP_BASE" "$DIR/$REPACK_DIR"
# Using a pipe hides tar errors!
tar cfC "$DIR/repacked.tar" "$DIR" "$REPACK_DIR"
xz -9 < "$DIR/repacked.tar" > "$DIR/repacked.tar.xz"

FILE=`dirname "$FILE"`/${PKG}_${LOCALVER}.orig.tar.xz
mv "$DIR/repacked.tar.xz" "$FILE"

echo "*** $FILE repackaged"

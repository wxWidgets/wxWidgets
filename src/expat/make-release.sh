#! /bin/bash
#
# make-release.sh: make an Expat release
#
# USAGE: make-release.sh tagname
#
# Note: tagname may be HEAD to just grab the head revision (e.g. for testing)
#

#CVSROOT=':ext:cvs.libexpat.org:/cvsroot/expat'
CVSROOT=':pserver:anonymous@expat.cvs.sourceforge.net:/cvsroot/expat'

if test $# != 1; then
  echo "USAGE: $0 tagname"
  exit 1
fi

tmpdir=expat-release.$$
if test -e $tmpdir; then
  echo "ERROR: oops. chose the $tmpdir subdir, but it exists."
  exit 1
fi

echo "Checking out into temporary area: $tmpdir"
cvs -fq -d "$CVSROOT" export -r "$1" -d $tmpdir expat || exit 1

echo ""
echo "----------------------------------------------------------------------"
echo "Preparing $tmpdir for release (running buildconf.sh)"
(cd $tmpdir && ./buildconf.sh) || exit 1

# figure out the release version
vsn="`$tmpdir/conftools/get-version.sh $tmpdir/lib/expat.h`"

echo ""
echo "Release version: $vsn"

if test "$1" = HEAD ; then
  distdir=expat-`date '+%Y-%m-%d'`
else
  distdir=expat-$vsn
fi
if test -e $distdir; then
  echo "ERROR: for safety, you must manually remove $distdir."
  rm -rf $tmpdir
  exit 1
fi
mkdir $distdir || exit 1

CPOPTS=-Pp
if (cp --version 2>/dev/null | grep -q 'Free Software Foundation') ; then
  # If we're using GNU cp, we can avoid the warnings about forward
  # compatibility of the options.
  CPOPTS='--parents --preserve'
fi

echo ""
echo "----------------------------------------------------------------------"
echo "Building $distdir based on the MANIFEST:"
files="`sed -e 's/[ 	]:.*$//' $tmpdir/MANIFEST`"
for file in $files; do
  echo "Copying $file..."
  (cd $tmpdir && cp $CPOPTS $file ../$distdir) || exit 1
done

echo ""
echo "----------------------------------------------------------------------"
echo "Removing (temporary) checkout directory..."
rm -rf $tmpdir

tarball=$distdir.tar.gz
echo "Constructing $tarball..."
tar cf - $distdir | gzip -9 > $tarball || exit $?
rm -r $distdir

echo "Done."

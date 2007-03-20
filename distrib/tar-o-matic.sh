#!/bin/sh

# create wx tarballs automatically given a tag, or from CVS HEAD
PROGNAME=$0
VERSION=""
TAG="" 
STARTDIR=$PWD

SPINWXX11=0
SPINWXGTK=0
SPINWXMOTIF=0
SPINWXMAC=0
SPINWXBASE=0
SPINWXMGL=0
SPINWXALL=0
SPINWXMSW=0
SPINDOCS=0
SPINEVERYTHING=0
REBAKE=0

SILENTFLAG=""

about()
{
	# sorry folks, I couldn't resist. :-)
	
	echo "It's the tar-o-matic 2000! It slices! It dices! It..."
	echo "Grabs CVS revisions and generates complete tarballs from them!!"
	echo ""
	echo "WARNING:"
	echo "- Do not use tar-o-matic while intoxicated."
	echo "- Do not use tar-o-matic while driving."
	echo "- Do not use tar-o-matic while driving intoxicated."
	echo "- Keep tar-o-matic away from small children. Large ones too."
	echo "- Tar-o-matic is not for use as a life-saving device."
	echo "- Do not call tar-o-matic nasty names. It really hates that."
	echo "- Avoid developing strong emotional attachment to tar-o-matic."
	echo "- Keep tar-o-matic away from unmonitored internet connections."
	echo "- Don't say we didn't warn you."
	echo ""
	echo "In case of malfunction, let tar-o-matic know that it is special."
}

usage()
{
    echo Usage: $PROGNAME " version-number cvs-revision [ options ]"
    echo Options:
    echo "    --help         Display this help message"
    echo "    --wxgtk        Spin wxGTK"
    echo "    --wxbase       Spin wxBase"
    echo "    --wxx11        Spin wxX11"
    echo "    --wxmotif      Spin wxMotif"
    echo "    --wxmac        Spin wxMac"
    echo "    --wxmgl        Spin wxMGL"
    echo "    --wxall        Spin wxALL"
    echo "    --wxmsw        Spin wxMSW"
    echo "    --docs         Spin docs"
    echo "    --all          Spin EVERYTHING"
    echo "    --updatecvs    Update from CVS"
    echo "    --silent       Don't prompt whenever possible"
    echo "                   only for use with revisions > 2.6.1"
    echo "    --about        about $PROGNAME"
    echo ""
    echo "NOTES:"
    echo "- Set EXTRASDIR environment variable to have any zip files"
    echo "  in that directory copied into deliver/extra for inclusion."
    echo "- Set WXDISTDIR environment variable to have completed "
    echo "  tarballs put in that directory. This allows for things like"
    echo "  setting a network path for all machines to send files to."
    echo "- EXTRASDIR defaults to ./extra and WXDISTDIR defaults to ./deliver"

    exit 1
}

if [ "$1" = "" ] || [ "$1" = "--help" ]; then
	usage
	exit 
fi

if [ "$1" = "--about" ]; then
	about
	exit
fi

VERSION=$1
shift 1

FINDTAG=`echo "$1" | sed /\-\-/p`
if [ FINDTAG != "" ]; then
  TAG=$1
  shift 1
fi

for i in "$@"; do
    case "$i" in
	--wxx11) SPINWXX11=1 ;;
	--wxgtk) SPINWXGTK=1 ;;
	--wxmac) SPINWXMAC=1 ;;
	--wxbase) SPINWXBASE=1 ;;
	--wxmgl) SPINWXMGL=1 ;;
	--wxmsw) SPINWXMSW=1 ;;
	--wxmotif) SPINWXMOTIF=1 ;;
	--docs) SPINDOCS=1 ;;
	--all) SPINEVERYTHING=1 ;;
	--rebake) REBAKE=1 ;;
	--silent) SILENTFLAG="--silent" ;;
	--clean-build) CLEANBUILD=1 ;;
	*)
	    usage
	    exit
	    ;;
    esac
done

# checkout wx sources to this tree
if [ -d ./temp-wx ]; then
	rm -rf ./temp-wx
fi

mkdir $STARTDIR/temp-wx
WXDIR="$STARTDIR/temp-wx/wxWidgets"

TAGNAME=""
if [ "$TAG" != "" ]; then
	echo "Checking out wxWidgets revision: $TAG"
	TAGNAME="-r $TAG"
else
	echo "Checking out wxWidgets HEAD sources..."
fi
echo "" # add a blank line for readability

cd $STARTDIR/temp-wx
cvs -d:pserver:anoncvs:anoncvs@cvs.wxwidgets.org:/pack/cvsroots/wxwidgets login

echo "cvs -d :pserver:anoncvs@cvs.wxwidgets.org:/pack/cvsroots/wxwidgets checkout $TAGNAME wxWidgets"
cvs -d :pserver:anoncvs@cvs.wxwidgets.org:/pack/cvsroots/wxwidgets checkout $TAGNAME wxWidgets

if [ "$REBAKE" = "1" ]; then
    echo "TODO! Implement re-baking..."
fi

# Copy extra .zips over to deliver/extra so they'll get picked up...
if [ ! -d $WXDIR/deliver ]; then
	mkdir $WXDIR/deliver
fi

if [ ! -d $WXDIR/deliver/extra ]; then
	mkdir $WXDIR/deliver/extra
fi

# allow the user to specify extras dir in an environment variable
EXTRA_DIR=$STARTDIR/extra
if [ "$EXTRASDIR" != "" ]; then
	EXTRA_DIR="$EXTRASDIR"
fi

for filename in $EXTRA_DIR/*.zip
do
	cp $filename $WXDIR/deliver/extra
done

if [ "$SPINWXMAC" = "1" ] || [ "$SPINEVERYTHING" = "1" ]; then
	cd $WXDIR
	sh $WXDIR/distrib/msw/tardist $WXDIR $WXDIR/deliver $VERSION --wxmac
fi

if [ "$SPINEVERYTHING" = "1" ]; then
	cd $WXDIR
	sh $WXDIR/distrib/msw/tardist $WXDIR $WXDIR/deliver $VERSION --wxall
fi

if [ "$SPINWXMSW" = "1" ] || [ "$SPINEVERYTHING" = "1" ]; then
	cd $WXDIR
	sh $WXDIR/distrib/msw/makesetup.sh --wxmsw
fi

if [ "$SPINWXBASE" = "1" ] || [ "$SPINEVERYTHING" = "1" ]; then
	cd $WXDIR
	sh $WXDIR/distrib/msw/maketarballs $WXDIR $WXDIR/deliver $VERSION --wxbase $SILENTFLAG
fi

if [ "$SPINWXOS2" = "1" ] || [ "$SPINEVERYTHING" = "1" ]; then
	cd $WXDIR
	sh $WXDIR/distrib/msw/makesetup.sh --wxos2
fi

if [ "$SPINWXGTK" = "1" ] || [ "$SPINEVERYTHING" = "1" ]; then
	cd $WXDIR
	sh $WXDIR/distrib/msw/maketarballs $WXDIR $WXDIR/deliver $VERSION --wxgtk $SILENTFLAG
fi

if [ "$SPINWXMOTIF" = "1" ] || [ "$SPINEVERYTHING" = "1" ]; then
	cd $WXDIR
	sh $WXDIR/distrib/msw/maketarballs $WXDIR $WXDIR/deliver $VERSION --wxmotif $SILENTFLAG
fi

if [ "$SPINWXMGL" = "1" ] || [ "$SPINEVERYTHING" = "1" ]; then
	cd $WXDIR
	sh $WXDIR/distrib/msw/maketarballs $WXDIR $WXDIR/deliver $VERSION --wxmgl $SILENTFLAG
fi

if [ "$SPINWXX11" = "1" ] || [ "$SPINEVERYTHING" = "1" ]; then
	cd $WXDIR
	sh $WXDIR/distrib/msw/maketarballs $WXDIR $WXDIR/deliver $VERSION --wxx11 $SILENTFLAG
fi

DISTDIR="$STARTDIR/deliver"
if [ "$WXDISTDIR" != "" ] && [ -d $WXDISTDIR ]; then
	DISTDIR="$WXDISTDIR"
fi

# delete any old tarballs
if [ -d $DISTDIR ]; then
	rm -rf $DISTDIR
fi

mkdir $DISTDIR

# copy over the created tarballs
cp $WXDIR/deliver/*.gz $DISTDIR
cp $WXDIR/deliver/*.bz2 $DISTDIR
cp $WXDIR/deliver/*.zip $DISTDIR

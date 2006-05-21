#!/bin/sh

# cleanup after old build files
#if [ -d $WX_TEMP_DIR ]; then
#  rm -rf $WX_TEMP_DIR
#fi 

START_DIR="$PWD"
WX_WEB_DIR=$WX_TEMP_DIR/wxWebSite
WX_SRC_DIR=$WX_TEMP_DIR/wxWidgets

# first, grab the latest revision with specified tag
if [ ! -d $WX_TEMP_DIR ]; then
  mkdir $WX_TEMP_DIR
fi 

cd $WX_TEMP_DIR

# just do an update if we started a build but it failed somewhere
if [ ! -d $WX_WEB_DIR ]; then  
  cvs -d:pserver:anoncvs:anoncvs@cvs.wxwidgets.org:/pack/cvsroots/wxwidgets login
  echo "Grabbing wxWebSite sources..."
  cvs -d:pserver:anoncvs@cvs.wxwidgets.org:/pack/cvsroots/wxwidgets checkout wxWebSite
else
    cd $WX_WEB_DIR
    cvs update -d
fi

if [ ! -d $WX_SRC_DIR ]; then
  cvs -d:pserver:anoncvs:anoncvs@cvs.wxwidgets.org:/pack/cvsroots/wxwidgets login
  echo "Grabbing wx CVS with tag $BUILD_TAG"
  cvs -d:pserver:anoncvs@cvs.wxwidgets.org:/pack/cvsroots/wxwidgets checkout -r $BUILD_TAG wxWidgets
else
    cd $WX_SRC_DIR
    cvs update -d
fi

# this is where we will store the wxAll tarball we create
if [ ! -d $DISTDIR ]; then
  mkdir $DISTDIR
fi

#re-bake the bakefiles
if [ $rebake = "yes" ]; then
  cd $WX_SRC_DIR/build/bakefiles
  bakefile_gen -d ../../distrib/scripts/Bakefiles.release.bkgen
  bakefile_gen -f watcom
  bakefile_gen -f watcom -d ../mgl/Bakefiles.mgl.bkgen
  bakefile_gen -f watcom -d ../os2/Bakefiles.os2.bkgen
fi

cd $WX_SRC_DIR
if [ ! -d $WX_SRC_DIR/deliver ]; then
  mkdir $WX_SRC_DIR/deliver
fi

# Now generate the mega tarball with everything. We will push this to our build machines.

cd $WX_TEMP_DIR
WX_TARBALL=$WX_TEMP_DIR/wxWidgets-$BUILD_VERSION.tar.gz
tar cvzf $WX_TARBALL wxWidgets

echo "Tarball located at: $WX_TARBALL"

if [ ! -f $WX_TARBALL ]; then
  echo "ERROR: wxAll tarball was not created by pre-flight.sh. Build cannot continue."
  exit 1
else
  cd $START_DIR
  cp $WX_TARBALL $STAGING_DIR
  cp -r $WX_WEB_DIR $STAGING_DIR
  
  echo "Pre-flight complete. Ready for takeoff."
fi

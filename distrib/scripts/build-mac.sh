#!/bin/bash
#----------------------------------------------------------------------

set -o errexit

if [ $skiplinux != yes ]; then
    # test if the target machine is online
    #if ping -q -c1 $WIN_HOST > /dev/null; then
	#echo " The $WIN_HOST machine is online, Windows build continuing..."
    #else
	#echo "The $WIN_HOST machine is **OFFLINE**, skipping the Windows build."
	#exit 0
    #fi

    echo "Copying source file and build script..."
    scp -r $STAGING_DIR/* $MAC_HOST:$MAC_BUILD
    
     echo "Untarring dist on $MAC_HOST..."
     wxtarball=$MAC_BUILD/wxWidgets-$BUILD_VERSION.tar.gz
     cmd="tar xzvf"
     ssh $MAC_HOST "cd $MAC_BUILD && $cmd $wxtarball"
     
     echo "Running build script on $MAC_HOST..."
     wxdir=$MAC_BUILD/wxWidgets
     cmd="./distrib/scripts/mac/tardist $wxdir $wxdir/deliver $BUILD_VERSION --wxmac --wxall --silent"
     ssh $MAC_HOST "cd $wxdir && chmod +x distrib/scripts/mac/tardist && $cmd"

     echo "Fetching the results..."
     scp "$MAC_HOST:$wxdir/deliver/*.tar.gz "  $DIST_DIR
     scp "$MAC_HOST:$wxdir/deliver/*.tar.bz2 "  $DIST_DIR

     echo "Done!"
fi
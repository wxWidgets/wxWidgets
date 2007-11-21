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
    scp -r $STAGING_DIR/* $LINUX_HOST:$LINUX_BUILD
    
     echo "Untarring dist on $LINUX_HOST..."
     wxtarball=$LINUX_BUILD/wxWidgets-$BUILD_VERSION.tar.gz
     cmd="tar xzvf"
     ssh $LINUX_HOST "cd $LINUX_BUILD && $cmd $wxtarball"
     
     echo "Running build script on $LINUX_HOST..."
     wxdir=$LINUX_BUILD/wxWidgets
     cmd="./distrib/scripts/unix/maketarballs $wxdir $wxdir/deliver $BUILD_VERSION --wxgtk --wxmotif --wxx11 --wxbase --wxmgl --silent"
     ssh $LINUX_HOST "cd $wxdir && chmod +x distrib/scripts/unix/maketarballs && $cmd"

     echo "Fetching the results..."
     scp "$LINUX_HOST:$wxdir/deliver/*.tar.gz "  $DIST_DIR
     scp "$LINUX_HOST:$wxdir/deliver/*.tar.bz2 "  $DIST_DIR

     echo "Done!"
fi
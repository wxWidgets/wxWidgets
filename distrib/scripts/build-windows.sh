#!/bin/bash
#----------------------------------------------------------------------

set -o errexit

if [ $skipwin != yes ]; then
    # test if the target machine is online
    #if ping -q -c1 $WIN_HOST > /dev/null; then
	#echo " The $WIN_HOST machine is online, Windows build continuing..."
    #else
	#echo "The $WIN_HOST machine is **OFFLINE**, skipping the Windows build."
	#exit 0
    #fi

    echo "Copying source file and build script..."
    scp -r $STAGING_DIR/* $WIN_HOST:$WIN_BUILD
    
     echo "Untarring dist on $WIN_HOST..."
     wxtarball=$WIN_BUILD/wxWidgets-$BUILD_VERSION.tar.gz
     cmd="tar xzvf"
     ssh $WIN_HOST "cd $WIN_BUILD && $cmd $wxtarball"
     
     echo "Running build script on $WIN_HOST..."
     wxdir=$WIN_BUILD/wxWidgets
     cmd="./makesetup.sh --wxmsw --wxos2 --wxbase --wxall --no-inno"
     ssh $WIN_HOST "cd $wxdir/distrib/scripts/msw && chmod +x makesetup.sh && WXWIN=$wxdir VERSION=$BUILD_VERSION $cmd"

     echo "Fetching the results..."
     scp "$WIN_HOST:$wxdir/deliver/*.zip "  $DIST_DIR
     #ssh $WIN_HOST "rm $WIN_BUILD/wxPython*-win32*"

     echo "Done!"
fi
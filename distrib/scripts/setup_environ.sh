#!/bin/bash

OLDDIR="$PWD"

# Install TaskRunner to manage the build process...
cd ../wxPython/distrib/all
sudo python2.4 setup.py install

cd $OLDDIR

BASE_CMD="wget"
if [ "$OSTYPE" = "darwin" ]; then
    BASE_CMD="curl -LO"
fi

if which ls > /dev/null 2>&1; then echo YES; else echo NO; fi

if which bogus > /dev/null 2>&1; then echo YES; else echo NO; fi

# unix2dos is needed for the build process.
# this commonly only happens on Mac...
U2D_TARBALL="unix2dos-2.2.src.tar.gz"
U2D_URL="http://opendarwin.org/~olegb/src/$U2D_TARBALL"

CMD="$BASE_CMD $U2D_URL"
echo "$CMD"
$CMD
    
tar xzvf $U2D_TARBALL
cd unix2dos-2.2.src
./configure
make
make install
cd ..


# Install ReleaseForge to manage the SF upload
RF_TARBALL="releaseforge-0.9.8.tar.gz"
RF_URL="http://voxel.dl.sourceforge.net/sourceforge/releaseforge/$RF_TARBALL"

if [ ! -d ./scripts/ReleaseForge ]; then    
    CMD="$BASE_CMD $RF_URL"
    echo "$CMD"
    $CMD
    
    tar xzvf $RF_TARBALL
    
    # There's one line of code that makes this not work from command line,
    # and that's an import that is no longer used, so we'll remove it. :-)
    cd "releaseforge-0.9.8"
    sed "s/from workerThread/\#from workerThread/" < ReleaseForge/sfcomm.py > temp
    mv temp ReleaseForge/sfcomm.py
    
    cd ..
    mv releaseforge-0.9.8/ReleaseForge ./scripts/ReleaseForge
    
    rm -rf releaseforge*
fi

BF_TARBALL="bakefile-0.2.0.tar.gz"
BF_URL="http://umn.dl.sourceforge.net/sourceforge/bakefile/$BF_TARBALL"

if which bakefile; then

else
    CMD="$BASE_CMD $BF_URL"
    echo "$CMD"
    $CMD
    
    tar xzvf $BF_TARBALL
    cd bakefile-0.2.0
    ./configure
    make
    sudo make install
fi
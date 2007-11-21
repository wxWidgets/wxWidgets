PATH="/c/Program Files/Bakefile/src":$PATH
cd $WXWIN
mkdir -p build/wince
echo "<?xml version=\"1.0\"?><bakefile-gen><enable-formats>msevc4prj</enable-formats></bakefile-gen>" > build/bakefiles/Bakefiles.local.bkgen
cd build/bakefiles
echo $PATH
# bakefile_gen -b wx.bkl -f msevc4prj
bakefile_gen -f msevc4prj


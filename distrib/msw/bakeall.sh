PATH="/c/Program Files/Bakefile/src":$PATH
cd $WXWIN
# echo "<?xml version=\"1.0\"?><bakefile-gen><enable-formats>msevc4prj,dmars</enable-formats></bakefile-gen>" > build/bakefiles/Bakefiles.local.bkgen
mkdir -p build/wince
cd build/bakefiles
echo $PATH
bakefile_gen


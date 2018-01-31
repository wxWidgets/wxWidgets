#!/bin/sh
#
# This script is used by Travis CI to configure, build and test wxWidgets

set -e

wxPROC_COUNT=`getconf _NPROCESSORS_ONLN`
((wxPROC_COUNT++))
wxJOBS="-j$wxPROC_COUNT"

echo 'Configuring...' && echo -en 'travis_fold:start:script.configure\\r'
./configure --disable-optimise $wxCONFIGURE_FLAGS
echo -en 'travis_fold:end:script.configure\\r'
echo 'Building...' && echo -en 'travis_fold:start:script.build\\r'
make $wxJOBS
echo -en 'travis_fold:end:script.build\\r'
echo 'Building tests...' && echo -en 'travis_fold:start:script.tests\\r'
make -C tests $wxJOBS
echo -en 'travis_fold:end:script.tests\\r'
echo 'Testing...' && echo -en 'travis_fold:start:script.testing\\r'
pushd tests && ./test -t && popd
echo -en 'travis_fold:end:script.testing\\r'
echo 'Building samples...' && echo -en 'travis_fold:start:script.samples\\r'
make samples
echo -en 'travis_fold:end:script.samples\\r'
echo 'Installing...' && echo -en 'travis_fold:start:script.install\\r'
sudo make install
echo -en 'travis_fold:end:script.install\\r'
echo 'Testing installation...' && echo -en 'travis_fold:start:script.testinstall\\r'
make -C samples/minimal -f makefile.unx clean
make -C samples/minimal -f makefile.unx $wxMAKEFILE_FLAGS
echo -en 'travis_fold:end:script.testinstall\\r'

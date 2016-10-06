#!/bin/sh
#
# This script is used by Travis CI to configure, build and test wxWidgets

set -e

echo 'travis_fold:start:configure'
echo 'Configuring...' 
./configure --disable-optimise $wxCONFIGURE_FLAGS
echo 'travis_fold:end:configure'
echo 'travis_fold:start:building'
echo 'Building...'
make
echo 'travis_fold:end:building'
echo 'travis_fold:start:testing'
echo 'Testing...'
make -C tests
pushd tests
./test -t
popd
echo 'travis_fold:end:testing'
echo 'travis_fold:start:samples'
echo 'Building the samples...'
make samples
echo 'travis_fold:end:samples'
echo 'travis_fold:start:installing'
echo 'Installing...'
sudo make install
echo 'travis_fold:end:installing'
echo 'travis_fold:start:test_install'
echo 'Testing building with the installed version...'
make -C samples/minimal -f makefile.unx $wxMAKEFILE_FLAGS
echo 'travis_fold:end:test_install'

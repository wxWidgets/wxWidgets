#!/bin/sh
#
# This script is used by Travis CI to configure, build and test wxWidgets

set -e

case $wxTOOLSET in
    cmake)
        if [ `uname -s` = "Linux" ] && [ `lsb_release -cs` = "precise" ]; then
            echo Updating CMake...
            wget -O - https://cmake.org/files/v3.6/cmake-3.6.2-Linux-x86_64.tar.gz | tar xzf -
            export PATH=`pwd`/cmake-3.6.2-Linux-x86_64/bin:$PATH
        fi

        if [ -z $wxCMAKE_TESTS ]; then wxCMAKE_TESTS=CONSOLE_ONLY; fi
        cmake --version
        echo 'travis_fold:start:configure'
        echo 'Configuring...'
        mkdir build_cmake
        pushd build_cmake
        cmake -G "$wxCMAKE_GENERATOR" $wxCMAKE_DEFINES -D wxBUILD_SAMPLES=SOME -D wxBUILD_TESTS=$wxCMAKE_TESTS .. 
        echo 'travis_fold:end:configure'
        echo 'travis_fold:start:building'
        echo 'Building...'
        cmake --build .
        echo 'travis_fold:end:building'
        if [ "$wxCMAKE_TESTS" != "OFF" ]; then
            echo 'travis_fold:start:testing'
            echo 'Testing...'
            ctest . -C Debug -V --output-on-failure
            echo 'travis_fold:end:testing'
        fi
        ;;
    *)
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
        ;;
esac

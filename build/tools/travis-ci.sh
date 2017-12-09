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
        echo 'Configuring...' && echo -en 'travis_fold:start:script.configure\\r'
        ./configure --disable-optimise $wxCONFIGURE_FLAGS
        echo -en 'travis_fold:end:script.configure\\r'
        echo 'Building...' && echo -en 'travis_fold:start:script.build\\r'
        make
        echo -en 'travis_fold:end:script.build\\r'
        echo 'Building tests...' && echo -en 'travis_fold:start:script.tests\\r'
        make -C tests
        echo -en 'travis_fold:end:script.tests\\r'
        echo 'Testing...' && echo -en 'travis_fold:start:script.testing\\r'
        pushd tests && ./test && popd
        echo -en 'travis_fold:end:script.testing\\r'
        echo 'Building samples...' && echo -en 'travis_fold:start:script.samples\\r'
        (test "$wxSKIP_SAMPLES" && echo 'SKIPPED') || make samples
        echo -en 'travis_fold:end:script.samples\\r'
        echo 'Installing...' && echo -en 'travis_fold:start:script.install\\r'
        sudo make install
        echo -en 'travis_fold:end:script.install\\r'
        echo 'Testing installation...' && echo -en 'travis_fold:start:script.testinstall\\r'
        make -C samples/minimal -f makefile.unx clean
        make -C samples/minimal -f makefile.unx $wxMAKEFILE_FLAGS
        echo -en 'travis_fold:end:script.testinstall\\r'
        ;;
esac

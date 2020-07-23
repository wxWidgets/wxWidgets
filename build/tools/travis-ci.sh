#!/bin/bash
#
# This script is used by Travis CI to configure, build and test wxWidgets

set -e

wxPROC_COUNT=`getconf _NPROCESSORS_ONLN`
((wxPROC_COUNT++))
wxBUILD_ARGS="-j$wxPROC_COUNT"

case $wxTOOLSET in
    cmake)
        if [ -z $wxCMAKE_TESTS ]; then wxCMAKE_TESTS=CONSOLE_ONLY; fi
        if [ -z $wxCMAKE_SAMPLES ]; then wxCMAKE_SAMPLES=SOME; fi
        if [ "$wxCMAKE_GENERATOR" == "Xcode" ]; then
            wxBUILD_ARGS="-jobs $wxPROC_COUNT -quiet"
        fi
        cmake --version

        echo 'travis_fold:start:configure'
        echo 'Configuring...'
        mkdir build_cmake
        pushd build_cmake
        cmake -G "$wxCMAKE_GENERATOR" $wxCMAKE_DEFINES -D wxBUILD_SAMPLES=$wxCMAKE_SAMPLES -D wxBUILD_TESTS=$wxCMAKE_TESTS ..
        echo 'travis_fold:end:configure'

        if [ "$wxCMAKE_GENERATOR" != "Xcode" ]; then
            echo 'travis_fold:start:building'
            echo 'Building...'
            cmake --build . -- $wxBUILD_ARGS
            echo 'travis_fold:end:building'
        fi

        echo 'travis_fold:start:install'
        if [ "$wxCMAKE_GENERATOR" == "Xcode" ]; then echo -n 'Building and '; fi
        echo 'Installing...'
        sudo env "PATH=$PATH" cmake --build . --target install -- $wxBUILD_ARGS
        popd
        echo 'travis_fold:end:install'

        if [ "$wxCMAKE_TESTS" != "OFF" ]; then
            echo 'travis_fold:start:testing'
            echo 'Testing...'
            ctest -V -C Debug -R "test_base" --output-on-failure --interactive-debug-mode 0 .
            echo 'travis_fold:end:testing'
        fi

        echo 'travis_fold:start:testinstall'
        echo 'Testing installation...'
        mkdir build_cmake_install_test
        pushd build_cmake_install_test
        cmake -G "$wxCMAKE_GENERATOR" $wxCMAKE_DEFINES ../samples/minimal
        cmake --build . -- $wxBUILD_ARGS
        popd
        echo 'travis_fold:end:testinstall'
        ;;
    *)
        echo 'travis_fold:start:configure'
        echo 'Configuring...'
        ./configure --disable-optimise --disable-debug_info $wxCONFIGURE_FLAGS || rc=$?
        if [ -n "$rc" ]; then
            echo '*** Configuring failed, contents of config.log follows: ***'
            echo '-----------------------------------------------------------'
            cat config.log
            echo '-----------------------------------------------------------'
            exit $rc
        fi
        echo 'travis_fold:end:configure'

        if [ "$wxALLOW_WARNINGS" != 1 ]; then
            case "$TRAVIS_COMPILER" in
                clang)
                    allow_warn_opt="-Wno-error=#warnings"
                    ;;

                gcc)
                    allow_warn_opt="-Wno-error=cpp"
                    ;;

                *)
                    echo "*** Unknown compiler: $TRAVIS_COMPILER ***"
                    ;;
            esac

            error_opts="-Werror $allow_warn_opt"
            wxMAKEFILE_CXXFLAGS="$wxMAKEFILE_CXXFLAGS $error_opts"
            wxMAKEFILE_ERROR_CXXFLAGS=("CXXFLAGS=$error_opts")
        else
            wxMAKEFILE_ERROR_CXXFLAGS=()
        fi

        wxMAKEFILE_FLAGS=()
        if [ -n "$wxMAKEFILE_CXXFLAGS" ]; then
            wxMAKEFILE_FLAGS+=("CXXFLAGS=$wxMAKEFILE_CXXFLAGS")
        fi
        if [ -n "$wxMAKEFILE_LDFLAGS" ]; then
            wxMAKEFILE_FLAGS+=("LDFLAGS=$wxMAKEFILE_LDFLAGS")
        fi

        echo 'travis_fold:start:building'
        echo 'Building...'
        make -k $wxBUILD_ARGS "${wxMAKEFILE_ERROR_CXXFLAGS[@]}"
        echo 'travis_fold:end:building'

        echo 'travis_fold:start:tests'
        echo 'Building tests...'
        [ "$wxSKIP_GUI" = 1 ] || make -C tests $wxBUILD_ARGS failtest
        make -k -C tests $wxBUILD_ARGS "${wxMAKEFILE_FLAGS[@]}"
        echo 'travis_fold:end:tests'

        if [ "$wxSKIP_TESTING" = 1 ]; then
            echo 'Skipping running tests'
            exit 0
        fi

        echo 'travis_fold:start:testing'
        echo 'Testing...'
        pushd tests
        ./test
        popd
        echo 'travis_fold:end:testing'

        if [ "$wxSKIP_GUI" = 1 ]; then
            echo 'Skipping the rest of tests for non-GUI build.'
            exit 0
        fi

        if [ "$wxUSE_XVFB" = 1 ]; then
            echo 'travis_fold:start:testing_gui'
            echo 'Testing GUI using Xvfb...'
            pushd tests
            xvfb-run -a -s '-screen 0 1600x1200x24' ./test_gui
            popd
            echo 'travis_fold:end:testing_gui'
        fi

        echo 'travis_fold:start:samples'
        echo 'Building samples...'
        (test "$wxSKIP_SAMPLES" && echo 'SKIPPED') || make -k "${wxMAKEFILE_FLAGS[@]}" samples
        echo 'travis_fold:end:samples'

        echo 'travis_fold:start:install'
        echo 'Installing...'
        sudo make install
        echo 'travis_fold:end:install'

        echo 'travis_fold:start:testinstall'
        echo 'Testing installation...'
        make -C samples/minimal -f makefile.unx clean
        make -C samples/minimal -f makefile.unx "${wxMAKEFILE_FLAGS[@]}"
        echo 'travis_fold:end:testinstall'
        ;;
esac

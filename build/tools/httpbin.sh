# This script is sourced by CI scripts to launch httpbin.
#
# Do not run it directly.

httpbin_launch() {
    WX_TEST_WEBREQUEST_URL=0
    export WX_TEST_WEBREQUEST_URL

    # We need python3 for httpbin, python2 can't be used any longer.
    if ! command -v python3 > /dev/null; then
        echo 'Python 3 is not available, not using httpbin.'
        return
    fi

    echo 'Launching httpbin...'

    case "$(uname -s)" in
        Linux)
            if command -v lsb_release > /dev/null; then
                dist_codename=$(lsb_release --codename --short)
            fi
            ;;

        Darwin)
            dist_codename='macOS'
            ;;
    esac

    case "$dist_codename" in
        trusty)
            # Explicitly select the versions of dependencies that are still
            # compatible with Python 3.4 used here as the latest versions of
            # several of them are not.
            pip_explicit_deps='Flask==1.0.4 Jinja2==2.10.3 MarkupSafe==1.1.1
            blinker==1.4 brotlipy==0.7.0 cffi==1.14.5 click==7.0 decorator==4.4.2
            itsdangerous==1.1.0 pycparser==2.20 raven==6.10.0 werkzeug==0.16.1'
            ;;

        *)
            # Installing Flask 2.1.0 and its dependency Werkzeug 2.1.0 results
            # in failures when trying to run httpbin, so stick to an older but
            # working version.
            pip_explicit_deps='Flask==2.0.3 Werkzeug==2.0.3'
    esac

    # Ensure that we have at least some version of pip and setuptools required
    # for installing cffi.
    if ! python3 -c 'import setuptools'; then
        sudo apt-get -q -o=Dpkg::Use-Pty=0 install python3-setuptools
    fi

    if ! python3 -c 'import pip'; then
        sudo apt-get -q -o=Dpkg::Use-Pty=0 install python3-pip
    fi

    # Running pip install fails with weird errors out of the box when
    # using old pip version because it attempts to use python rather
    # than python3, so upgrade it to fix this.
    #
    # However don't upgrade to a version which is too new because then
    # it may not support Python version that we actually have (this one
    # still works with 3.4, 20.0.1 is the last one to support 3.5).
    python3 -m pip install --user --upgrade pip==19.1.1
    python3 -m pip install --user wheel

    echo "Installing using `python3 -m pip --version`"

    python3 -m pip install $pip_explicit_deps httpbin --user
    python3 -m httpbin.core --port 50500 2>&1 >httpbin.log &
    WX_TEST_WEBREQUEST_URL="http://localhost:50500"
}

httpbin_show_log() {
    if [ "$WX_TEST_WEBREQUEST_URL" != "0" ]; then
        echo '*** Tests failed, contents of httpbin.log follows: ***'
        echo '-----------------------------------------------------------'
        cat httpbin.log
        echo '-----------------------------------------------------------'
    fi
}

# This script is sourced by CI scripts to launch httpbin.
#
# Do not run it directly.

httpbin_launch() {
    echo 'Launching httpbin...'

    case "$(uname -s)" in
        Linux)
            dist_codename=$(lsb_release --codename --short)
            ;;

        Darwin)
            dist_codename='macOS'
            ;;
    esac

    case "$dist_codename" in
        trusty)
            # Python 2.7.6 is too old to support SNI and can't be used.
            PY3=3

            # Current decorator (>= 5) is incompatible with Python 3.4 used
            # here, so explicitly use a version which is known to work.
            # Similarly, werkzeug >= 1 doesn't support 3.4 any longer.
            pip_explicit_deps='decorator==4.4.2 werkzeug==0.16.1'
            ;;

        macOS)
            # We use Python 2 under macOS 10.11 which doesn't have Python 3,
            # and decorator >= 5 is incompatible with it too.
            pip_explicit_deps='decorator==4.4.2'
            ;;

        *)
            # Elsewhere just use Python 3.
            PY3=3
            ;;
    esac

    if [ "$PY3" = 3 ]; then
        # Ensure that we have at least some version of pip.
        if ! python3 -m pip; then
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
    fi

    echo "Installing using `python$PY3 -m pip --version`"

    python$PY3 -m pip install $pip_explicit_deps httpbin --user
    python$PY3 -m httpbin.core 2>&1 >httpbin.log &
    WX_TEST_WEBREQUEST_URL="http://localhost:5000"

    export WX_TEST_WEBREQUEST_URL
}

httpbin_show_log() {
    echo '*** Tests failed, contents of httpbin.log follows: ***'
    echo '-----------------------------------------------------------'
    cat httpbin.log
    echo '-----------------------------------------------------------'
}

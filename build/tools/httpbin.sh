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
            # Current decorator (>= 5) is incompatible with Python 2 which we
            # still use in some builds, so explicitly use a version which
            # is known to work. Just specifying "decorator==4.4.2" doesn't work
            # neither for some reason ("no matching distribution found"), so
            # use an explicit URL.
            pip_decorator_arg='https://files.pythonhosted.org/packages/ed/1b/72a1821152d07cf1d8b6fce298aeb06a7eb90f4d6d41acec9861e7cc6df0/decorator-4.4.2-py2.py3-none-any.whl'

            # We need to disable SSL certificate checking under Trusty because
            # Python version there is too old to support SNI.
            pip_options='--trusted-host files.pythonhosted.org'
            ;;

        macOS)
            pip_decorator_arg='decorator==4.4.2'
            ;;

        *)
            # Elsewhere just use Python 3.
            PY3=3

            # Ensure that we have at least some version of pip.
            if ! python3 -m pip; then
                sudo apt-get -q -o=Dpkg::Use-Pty=0 install python3-pip
            fi

            # Running pip install fails with weird errors out of the box when
            # using old pip version because it attempts to use python rather
            # than python3, so upgrade it to fix this.
            #
            # However don't upgrade to a version which is too new because then
            # it may not support Python version that we actually have.
            python3 -m pip install --user --upgrade pip==20.0.1
            ;;
    esac

    echo "Installing using `python$PY3 -m pip --version`"

    python$PY3 -m pip install $pip_decorator_arg httpbin --user $pip_options
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

# This script is sourced by CI scripts to launch httpbin.
#
# Do not run it directly.

httpbin_launch() {
    echo 'Launching httpbin...'

    case "$(uname -s)" in
        Linux)
            dist_codename=$(lsb_release --codename --short)
            ;;
    esac

    # We need to disable SSL certificate checking under Trusty because Python
    # version there is too old to support SNI.
    case "$dist_codename" in
        trusty)
            # Here "decorator==4.4.2" doesn't work neither for some reason ("no
            # matching distribution found"), so use an explicit URL.
            pip_decorator_arg='https://files.pythonhosted.org/packages/ed/1b/72a1821152d07cf1d8b6fce298aeb06a7eb90f4d6d41acec9861e7cc6df0/decorator-4.4.2-py2.py3-none-any.whl'
            pip_options='--trusted-host files.pythonhosted.org'
            ;;

        *)
            # Current decorator (>= 5) is incompatible with Python 2 which we
            # still use under in some builds, so explicitly use a version which
            # is known to work.
            pip_decorator_arg='decorator==4.4.2'
            ;;
    esac

    pip install $pip_decorator_arg httpbin --user $pip_options
    python -m httpbin.core 2>&1 >httpbin.log &
    WX_TEST_WEBREQUEST_URL="http://localhost:5000"

    export WX_TEST_WEBREQUEST_URL
}

httpbin_show_log() {
    echo '*** Tests failed, contents of httpbin.log follows: ***'
    echo '-----------------------------------------------------------'
    cat httpbin.log
    echo '-----------------------------------------------------------'
}

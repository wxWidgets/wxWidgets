# This script is sourced by CI scripts to launch httpbin.
#
# Do not run it directly.

httpbin_launch() {
    WX_TEST_WEBREQUEST_URL=0
    export WX_TEST_WEBREQUEST_URL

    echo 'Launching httpbin...'

    # Installing Flask 2.1.0 and its dependency Werkzeug 2.1.0 results
    # in failures when trying to run httpbin, so stick to an older but
    # working version.
    pip_explicit_deps='Flask==2.0.3 Werkzeug==2.0.3'

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

# This script is sourced by CI scripts to launch httpbin.
#
# Do not run it directly.

httpbin_launch() {
    # If the tests are already disabled, don't do anything.
    if [ "$WX_TEST_WEBREQUEST_URL" = "0" ]; then
        return 0
    fi

    # Disable the tests by default and only reenable them below if we do manage
    # to launch httpbin.
    WX_TEST_WEBREQUEST_URL=0
    export WX_TEST_WEBREQUEST_URL

    go version
    if ! go install github.com/mccutchen/go-httpbin/v2/cmd/go-httpbin@v2; then
        echo '::warning::Failed to install httpbin, skipping wxWebRequest tests.'
        return 0
    fi

    echo 'Launching httpbin...'
    go-httpbin -host 127.0.0.1 -port 8081 >httpbin.log 2>&1 &
    WX_TEST_WEBREQUEST_URL="http://127.0.0.1:8081"
}

httpbin_show_log() {
    if [ "$WX_TEST_WEBREQUEST_URL" != "0" ]; then
        echo '*** Tests failed, contents of httpbin.log follows: ***'
        echo '-----------------------------------------------------------'
        cat httpbin.log
        echo '-----------------------------------------------------------'
    fi
}

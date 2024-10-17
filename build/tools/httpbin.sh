# This script is sourced by CI scripts to launch httpbin.
#
# Do not run it directly.

httpbin_launch() {
    WX_TEST_WEBREQUEST_URL=0
    export WX_TEST_WEBREQUEST_URL

    go version
    go install github.com/mccutchen/go-httpbin/v2/cmd/go-httpbin@v2

    echo 'Launching httpbin...'
    go-httpbin -host 127.0.0.1 -port 8081 2>&1 >httpbin.log &
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

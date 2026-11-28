# This script is sourced by CI scripts to launch httpbin.
#
# Do not run it directly.

# Launch httpbin on a free port and set WX_TEST_WEBREQUEST_URL to its URL or to
# "0", disabling the tests using it, if it couldn't be launched.
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

    # Use curl for checking that httpbin really works, if it's available, as
    # it's not guaranteed to be installed in the CI containers.
    httpbin_curl=$(command -v curl || true)

    # Don't use a fixed port as it may be already in use by something else, but
    # rather a random one -- and retry with another one if we're unlucky.
    httpbin_port=$(awk 'BEGIN{srand(); print 10000 + int(rand()*20000)}')

    httpbin_attempt=0
    while [ $httpbin_attempt -lt 5 ]; do
        httpbin_attempt=$((httpbin_attempt + 1))

        echo "Launching httpbin on port $httpbin_port..."
        go-httpbin -host 127.0.0.1 -port $httpbin_port >httpbin.log 2>&1 &
        httpbin_pid=$!

        # Give it some time to start up (or to fail to do it).
        httpbin_wait=0
        while [ $httpbin_wait -lt 20 ]; do
            httpbin_wait=$((httpbin_wait + 1))
            sleep 0.5

            # Check that our process is still running first: if the port was
            # busy, it has already exited by now and whatever is using this
            # port could answer the request made below.
            if ! kill -0 $httpbin_pid 2>/dev/null; then
                break
            fi

            # If we can't check anything else, assume that it works, as it
            # would have exited if it couldn't bind to the port.
            if [ -z "$httpbin_curl" ]; then
                WX_TEST_WEBREQUEST_URL="http://127.0.0.1:$httpbin_port"
                break
            fi

            if "$httpbin_curl" --silent --show-error --output /dev/null \
                    "http://127.0.0.1:$httpbin_port/status/200"; then
                WX_TEST_WEBREQUEST_URL="http://127.0.0.1:$httpbin_port"
                break
            fi
        done

        if [ "$WX_TEST_WEBREQUEST_URL" != "0" ]; then
            break
        fi

        echo '*** Launching httpbin failed, contents of httpbin.log follows: ***'
        echo '-----------------------------------------------------------'
        cat httpbin.log
        echo '-----------------------------------------------------------'

        # It could still be running but just not answering, don't leave it
        # behind in this case.
        kill $httpbin_pid 2>/dev/null || true

        httpbin_port=$((httpbin_port + 1))
    done

    if [ "$WX_TEST_WEBREQUEST_URL" = "0" ]; then
        echo '::warning::Failed to launch httpbin, skipping wxWebRequest tests.'
    fi

    return 0
}

httpbin_show_log() {
    if [ "$WX_TEST_WEBREQUEST_URL" != "0" ]; then
        echo '*** Tests failed, contents of httpbin.log follows: ***'
        echo '-----------------------------------------------------------'
        cat httpbin.log
        echo '-----------------------------------------------------------'
    fi
}

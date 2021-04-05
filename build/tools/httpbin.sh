# This script is sourced by CI scripts to launch httpbin.
#
# Do not run it directly.

httpbin_launch() {
    echo 'Launching httpbin...'

    case "$(uname -s)" in
        Linux)
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
            ;;

        Darwin)
            ;;
    esac

    echo "Installing httpbin using `python3 -m pip --version`"

    python3 -m pip install httpbin --user
    python3 -m httpbin.core 2>&1 >httpbin.log &
    WX_TEST_WEBREQUEST_URL="http://localhost:5000"

    export WX_TEST_WEBREQUEST_URL
}

httpbin_show_log() {
    echo '*** Tests failed, contents of httpbin.log follows: ***'
    echo '-----------------------------------------------------------'
    cat httpbin.log
    echo '-----------------------------------------------------------'
}

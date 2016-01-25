#!/bin/sh
#
# This script is used by Travis CI to install the dependencies before building
# wxWidgets but can also be run by hand if necessary but currently it only
# works for Ubuntu 12.04 and 14.04 and OS X used by Travis builds.

SUDO=sudo

case $(uname -s) in
    Linux)
        if [ -f /etc/apt/sources.list ]; then
            $SUDO apt-get update
            $SUDO apt-get install -y libcppunit-dev libgtk2.0-dev libnotify-dev
        fi
        ;;

    Darwin)
        brew update
        brew install cppunit --universal
        ;;
esac

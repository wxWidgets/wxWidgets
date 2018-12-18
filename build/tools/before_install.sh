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
            case "$wxGTK_VERSION" in
                3) libgtk_dev=libgtk-3-dev ;;
                *) libgtk_dev=libgtk2.0-dev;;
            esac

            case "$wxCONFIGURE_FLAGS" in
                *--with-directfb*) libtoolkit_dev='libdirectfb-dev'         ;;
                *--with-motif*)    libtoolkit_dev='libmotif-dev libxmu-dev' ;;
                *--with-qt*)       libtoolkit_dev='qtdeclarative5-dev'      ;;
            esac

            pgk_check="libgstreamermm-1.0-dev libgstreamermm-0.10-dev \
                libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
                libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev \
                libwebkitgtk-dev libglu1-mesa-dev libsecret-1-dev libnotify-dev"

            for pkg in $pgk_check; do
                if $(apt-cache pkgnames | grep -q $pkg) ; then
                    pkg_install="$pkg_install $pkg"
                fi
            done

            $SUDO apt-get install -y $libgtk_dev $libtoolkit_dev $pkg_install
        fi
        ;;

    Darwin)
        ;;
esac

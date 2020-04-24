#!/bin/sh
#
# This script is used by Travis CI to install the dependencies before building
# wxWidgets but can also be run by hand if necessary but currently it only
# works for Ubuntu versions used by Travis builds.

SUDO=sudo

case $(uname -s) in
    Linux)
        if [ -f /etc/apt/sources.list ]; then
            $SUDO apt-get update

            case "$wxCONFIGURE_FLAGS" in
                *--with-directfb*) libtoolkit_dev='libdirectfb-dev'         ;;
                *--with-motif*)    libtoolkit_dev='libmotif-dev libxmu-dev' ;;
                *--with-qt*)       libtoolkit_dev='qtdeclarative5-dev libqt5opengl5-dev';;
                *--with-x11*)      extra_deps='libpango1.0-dev' ;;
                *--disable-gui*)   ;;
                *)
                    case "$wxGTK_VERSION" in
                        3)  libtoolkit_dev=libgtk-3-dev
                            extra_deps='libwebkit2gtk-4.0-dev libwebkitgtk-3.0-dev'
                            ;;
                        *)  libtoolkit_dev=libgtk2.0-dev
                            extra_deps='libwebkitgtk-dev'
                            ;;
                    esac

                    extra_deps="$extra_deps \
                            libgstreamermm-1.0-dev libgstreamermm-0.10-dev \
                            libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
                            libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev \
                            libglu1-mesa-dev"
            esac

            extra_deps="$extra_deps libsecret-1-dev libnotify-dev"
            for pkg in $extra_deps; do
                if $(apt-cache pkgnames | grep -q $pkg) ; then
                    pkg_install="$pkg_install $pkg"
                fi
            done

            $SUDO apt-get install -y $libtoolkit_dev $pkg_install
        fi
        ;;

    Darwin)
        ;;
esac

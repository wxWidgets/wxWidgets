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
                        2)  libtoolkit_dev=libgtk2.0-dev
                            extra_deps='libwebkitgtk-dev'
                            ;;
                        *)  echo 'Please specify wxGTK_VERSION explicitly.' >&2
                            exit 1
                            ;;
                    esac

                    extra_deps="$extra_deps \
                            libgstreamermm-1.0-dev libgstreamermm-0.10-dev \
                            libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
                            libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev \
                            libglu1-mesa-dev"
            esac

            extra_deps="$extra_deps libcurl4-openssl-dev libsecret-1-dev libnotify-dev"
            for pkg in $extra_deps; do
                if $(apt-cache pkgnames | grep -q $pkg) ; then
                    pkg_install="$pkg_install $pkg"
                fi
            done

            $SUDO apt-get install -y $libtoolkit_dev $pkg_install

            if [ "$wxUSE_ASAN" = 1 ]; then
                codename=$(lsb_release --codename --short)
                # Enable the `-dbgsym` repositories.
                echo "deb http://ddebs.ubuntu.com ${codename} main restricted universe multiverse
                deb http://ddebs.ubuntu.com ${codename}-updates main restricted universe multiverse
                deb http://ddebs.ubuntu.com ${codename}-proposed main restricted universe multiverse" | \
                $SUDO tee --append /etc/apt/sources.list.d/ddebs.list

                # Import the debug symbol archive signing key from the Ubuntu server.
                # Note that this command works only on Ubuntu 18.04 LTS and newer.
                $SUDO apt-get install -y ubuntu-dbgsym-keyring

                $SUDO apt-get update

                # Install the symbols to allow LSAN suppression list to work.
                $SUDO apt-get install -y libfontconfig1-dbgsym libglib2.0-0-dbgsym libgtk-3-0-dbgsym libatk-bridge2.0-0-dbgsym
            fi
        fi
        ;;

    Darwin)
        ;;
esac

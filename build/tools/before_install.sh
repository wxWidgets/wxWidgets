#!/bin/sh
#
# This script is used by Travis CI to install the dependencies before building
# wxWidgets but can also be run by hand if necessary but currently it only
# works for Ubuntu versions used by Travis builds.

set -e

SUDO=sudo

case $(uname -s) in
    Linux)
        if [ -f /etc/apt/sources.list ]; then
            # Show information about the repositories and priorities used.
            echo 'APT sources used:'
            $SUDO grep --no-messages '^[^#]' /etc/apt/sources.list /etc/apt/sources.list.d/* || true
            echo 'APT preferences:'
            $SUDO grep --no-messages '^[^#]' /etc/apt/preferences /etc/apt/preferences.d/* || true
            echo '--- End of APT files dump ---'

            run_apt() {
                echo "Running apt-get $@"

                # Disable some (but not all) output.
                $SUDO apt-get -q -o=Dpkg::Use-Pty=0 "$@"

            }

            if [ "$wxUSE_ASAN" = 1 ]; then
                codename=$(lsb_release --codename --short)
                # Enable the `-dbgsym` repositories.
                echo "deb http://ddebs.ubuntu.com ${codename} main restricted universe multiverse
                deb http://ddebs.ubuntu.com ${codename}-updates main restricted universe multiverse" | \
                $SUDO tee --append /etc/apt/sources.list.d/ddebs.list >/dev/null

                # Import the debug symbol archive signing key from the Ubuntu server.
                # Note that this command works only on Ubuntu 18.04 LTS and newer.
                run_apt install -y ubuntu-dbgsym-keyring

                # The key in the package above is currently (2021-03-22) out of
                # date, so get the latest key manually (this is completely
                # insecure, of course, but we don't care).
                wget -O - http://ddebs.ubuntu.com/dbgsym-release-key.asc | $SUDO apt-key add -

                # Install the symbols to allow LSAN suppression list to work.
                dbgsym_pkgs='libfontconfig1-dbgsym libglib2.0-0-dbgsym libgtk-3-0-dbgsym libatk-bridge2.0-0-dbgsym'
            fi

            run_apt update || echo 'Failed to update packages, but continuing nevertheless.'

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

            pkg_install="$pkg_install $libtoolkit_dev gdb"

            extra_deps="$extra_deps libcurl4-openssl-dev libsecret-1-dev libnotify-dev"
            for pkg in $extra_deps; do
                if $(apt-cache pkgnames | grep -q $pkg) ; then
                    pkg_install="$pkg_install $pkg"
                else
                    echo "Not installing non-existent package $pkg"
                fi
            done

            if ! run_apt install -y $pkg_install $dbgsym_pkgs; then
                if [ -z "$dbgsym_pkgs" ]; then
                    exit $?
                fi

                # Retry without dbgsym packages that currently fail to install
                # under Ubuntu Focal (20.04).
                echo 'Installing with dbgsym packages failed, retrying without...'
                run_apt install -y $pkg_install
            else
                touch wx_dbgsym_available
            fi
        fi
        ;;

    Darwin)
        ;;
esac

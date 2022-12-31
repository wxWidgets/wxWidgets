#!/bin/sh
#
# This script is used by CI jobs to install the dependencies
# before building wxWidgets but can also be run by hand if necessary (but
# currently it only works for the OS versions used by the CI builds).
#
# WX_EXTRA_PACKAGES environment variable may be predefined to contain extra
# packages to install (in an OS-specific way) in addition to the required ones.

set -e

case $(uname -s) in
    Linux)
        # Use sudo if it's available or assume root otherwise.
        if command -v sudo > /dev/null; then
            SUDO=sudo
        else
            if [ `id -u` -ne 0 ]; then
                echo "Please install sudo or run as root (and not user `id -u`)." >& 2
                exit 1
            fi
        fi

        # Debian/Ubuntu
        if [ -f /etc/apt/sources.list ]; then
            # Show information about the repositories and priorities used.
            echo 'APT sources used:'
            $SUDO grep --no-messages '^[^#]' /etc/apt/sources.list /etc/apt/sources.list.d/* || true
            echo '--- End of APT files dump ---'

            run_apt() {
                echo "-> Running apt-get $@"

                # Disable some (but not all) output.
                $SUDO apt-get -q -o=Dpkg::Use-Pty=0 "$@"

                rc=$?
                echo "-> Done with $rc"

                return $rc
            }

            # We could install lsb-release package if the command is missing,
            # but we currently only actually use codename on the systems where
            # it's guaranteed to be installed, so don't bother doing it for now.
            if command -v lsb_release > /dev/null; then
                codename=$(lsb_release --codename --short)
            fi

            if [ "$wxUSE_ASAN" = 1 ]; then
                # Enable the `-dbgsym` repositories.
                echo "deb http://ddebs.ubuntu.com ${codename} main restricted universe multiverse
                deb http://ddebs.ubuntu.com ${codename}-updates main restricted universe multiverse" | \
                $SUDO tee --append /etc/apt/sources.list.d/ddebs.list >/dev/null

                # Import the debug symbol archive signing key from the Ubuntu server.
                # Note that this command works only on Ubuntu 18.04 LTS and newer.
                run_apt install -y ubuntu-dbgsym-keyring

                # Install the symbols to allow LSAN suppression list to work.
                dbgsym_pkgs='libfontconfig1-dbgsym libglib2.0-0-dbgsym libgtk-3-0-dbgsym libatk-bridge2.0-0-dbgsym'
            fi

            run_apt update || echo 'Failed to update packages, but continuing nevertheless.'

            case "$wxCONFIGURE_FLAGS" in
                *--with-directfb*) libtoolkit_dev='libdirectfb-dev'         ;;
                *--with-qt*)       libtoolkit_dev='qtdeclarative5-dev libqt5opengl5-dev';;
                *--with-x11*)      extra_deps='libpango1.0-dev' ;;
                *--disable-gui*)   ;;
                *)
                    case "$wxGTK_VERSION" in
                        3)  libtoolkit_dev=libgtk-3-dev
                            extra_deps='libwebkit2gtk-4.0-dev libgspell-1-dev'
                            ;;
                        2)  libtoolkit_dev=libgtk2.0-dev
                            extra_deps='libwebkitgtk-dev'
                            ;;
                        *)  echo 'Please specify wxGTK_VERSION explicitly.' >&2
                            exit 1
                            ;;
                    esac

                    case "$codename" in
                        jammy)
                            # Under Ubuntu 22.04 installing libgstreamer1.0-dev
                            # fails because it depends on libunwind-dev which
                            # is not going to be installed because it conflicts
                            # with the pre-installed (in GitHub Actions
                            # environment) libc++-dev, so we need to install it
                            # directly to avoid errors later.
                            extra_deps="$extra_deps libunwind-dev"
                            ;;
                    esac

                    extra_deps="$extra_deps \
                            libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
                            libglu1-mesa-dev"
            esac

            # Install locales used by our tests to run all the tests instead of
            # skipping them.
            pkg_install="$pkg_install $libtoolkit_dev gdb locales-all ${WX_EXTRA_PACKAGES}"

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
                    exit 1
                fi

                # Retry without dbgsym packages that currently fail to install
                # under Ubuntu Focal (20.04).
                echo 'Installing with dbgsym packages failed, retrying without...'
                if ! run_apt install -y $pkg_install; then
                    exit 1
                fi
            else
                touch wx_dbgsym_available
            fi
        fi

        if [ -f /etc/redhat-release ]; then
            dnf install -y ${WX_EXTRA_PACKAGES} expat-devel findutils g++ git-core gspell-devel gstreamer1-plugins-base-devel gtk3-devel make libcurl-devel libGLU-devel libjpeg-devel libnotify-devel libpng-devel libSM-devel libsecret-devel libtiff-devel SDL-devel webkit2gtk3-devel zlib-devel
        fi
        ;;

    FreeBSD)
        pkg install -q -y ${WX_EXTRA_PACKAGES} gspell gstreamer1 gtk3 jpeg-turbo libnotify libsecret mesa-libs pkgconf png tiff webkit2-gtk3
        ;;

    Darwin)
        ;;
esac

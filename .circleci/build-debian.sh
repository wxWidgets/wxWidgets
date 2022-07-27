#!/usr/bin/env bash

#
# Build for Debian in a docker container
#

# bailout on errors and echo commands.
set -xe

DOCKER_SOCK="unix:///var/run/docker.sock"

echo "DOCKER_OPTS=\"-H tcp://127.0.0.1:2375 -H $DOCKER_SOCK -s overlay2\"" | sudo tee /etc/default/docker > /dev/null
sudo service docker restart
sleep 5;

if [ "$EMU" = "on" ]; then
  if [ "$CONTAINER_DISTRO" = "raspbian" ]; then
      docker run --rm --privileged multiarch/qemu-user-static:register --reset
  else
      docker run --rm --privileged --cap-add=ALL --security-opt="seccomp=unconfined" multiarch/qemu-user-static --reset --credential yes --persistent yes
  fi
fi

WORK_DIR=$(pwd):/ci-source

docker run --privileged --cap-add=ALL --security-opt="seccomp=unconfined" -d -ti -e "container=docker"  -v $WORK_DIR:rw $DOCKER_IMAGE /bin/bash
DOCKER_CONTAINER_ID=$(docker ps --last 4 | grep $CONTAINER_DISTRO | awk '{print $1}')

docker exec --privileged -ti $DOCKER_CONTAINER_ID apt-get update
docker exec --privileged -ti $DOCKER_CONTAINER_ID apt-get -y install apt-transport-https wget curl gnupg2

#docker exec --privileged -ti $DOCKER_CONTAINER_ID /bin/bash -xec \
#  "wget -q 'https://dl.cloudsmith.io/public/bbn-projects/bbn-repo/cfg/gpg/gpg.070C975769B2A67A.key' -O- | apt-key add -"
#docker exec --privileged -ti $DOCKER_CONTAINER_ID /bin/bash -xec \
#  "wget -q 'https://dl.cloudsmith.io/public/bbn-projects/bbn-repo/cfg/setup/config.deb.txt?distro=${PKG_DISTRO}&codename=${PKG_RELEASE}' -O- | tee -a /etc/apt/sources.list"

docker exec --privileged -ti $DOCKER_CONTAINER_ID apt-get upgrade
docker exec --privileged -ti $DOCKER_CONTAINER_ID apt-get -y install dpkg-dev devscripts equivs pkg-config apt-utils fakeroot
docker exec --privileged -ti $DOCKER_CONTAINER_ID apt-get -y install autotools-dev autoconf dh-exec cmake gettext git-core \
    libgtk-3-dev                           \
    libgtk2.0-dev                          \
    libgl1-mesa-dev                        \
    libglu1-mesa-dev                       \
    zlib1g-dev                             \
    libjpeg-dev                            \
    libpng-dev                             \
    libtiff5-dev                           \
    libsm-dev                              \
    libexpat1-dev                          \
    libxt-dev                              \
    libgstreamer1.0-dev                    \
    libgstreamer-plugins-base1.0-dev       \
    libgstreamer-plugins-bad1.0-dev        \
    libwebkit2gtk-4.0-dev                  \
    libnotify-dev                          \
    wget                                   \
    doxygen                                \
    graphviz                               \
    meson                                  \
    debhelper-compat                       \
    bc                                     \
    bison                                  \
    flex                                   \
    at-spi2-core                           \
    libglib2.0-doc                         \
    libatk-bridge2.0-dev                   \
    libatk1.0-dev                          \
    libcairo2-dev                          \
    libegl1-mesa-dev                       \
    libepoxy-dev                           \
    libfontconfig1-dev                     \
    libfribidi-dev                         \
    libharfbuzz-dev                        \
    libpango1.0-dev                        \
    libwayland-dev                         \
    libxcomposite-dev                      \
    libxcursor-dev                         \
    libxdamage-dev                         \
    libxext-dev                            \
    libxfixes-dev                          \
    libxi-dev                              \
    libxinerama-dev                        \
    libxkbcommon-dev                       \
    libxml2-utils                          \
    libxrandr-dev                          \
    wayland-protocols                      \
    libatk1.0-doc                          \
    libpango1.0-doc                        \
    adwaita-icon-theme                     \
    dh-sequence-gir                        \
    fonts-cantarell                        \
    gnome-pkg-tools                        \
    gobject-introspection                  \
    gsettings-desktop-schemas              \
    gtk-doc-tools                          \
    libcolord-dev                          \
    libcups2-dev                           \
    libgdk-pixbuf2.0-dev                   \
    libgirepository1.0-dev                 \
    libjson-glib-dev                       \
    librest-dev                            \
    librsvg2-common                        \
    libxkbfile-dev                         \
    sassc                                  \
    xauth                                  \
    xvfb                                   \
    docbook-xml                            \
    docbook-xsl                            \
    libcairo2-doc                          \
    libc6                                  \
    libglib2.0-0                           \
    libjson-glib-1.0-0                     \
    libxcomposite1                         \
    gir1.2-gtk-3.0                         \
    gtk-update-icon-cache                  \
    xsltproc

docker exec --privileged -ti $DOCKER_CONTAINER_ID ldconfig

docker exec --privileged -ti $DOCKER_CONTAINER_ID /bin/bash -xec \
    "update-alternatives --set fakeroot /usr/bin/fakeroot-tcp; cd ci-source; dpkg-buildpackage -uc -us -j8; mkdir dist; mv ../*.deb dist; chmod -R a+rw dist"

find dist -name \*.\*$EXT

echo "Stopping"
docker ps -a
docker stop $DOCKER_CONTAINER_ID
docker rm -v $DOCKER_CONTAINER_ID

#!/usr/bin/env bash

#
# Build for Debian in a docker container
#

# bailout on errors and echo commands.
set -xe

DOCKER_SOCK="unix:///var/run/docker.sock"

echo "DOCKER_OPTS=\"-H tcp://127.0.0.1:2375 -H $DOCKER_SOCK -s devicemapper\"" | sudo tee /etc/default/docker > /dev/null
sudo service docker restart
sleep 5;

if [ "$EMU" = "on" ]; then
  if [ "$CONTAINER_DISTRO" = "raspbian" ]; then
      docker run --rm --privileged multiarch/qemu-user-static:register --reset
  else
      docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
  fi
fi

WORK_DIR=$(pwd):/ci-source

docker run --privileged -d -ti -e "container=docker"  -v $WORK_DIR:rw $DOCKER_IMAGE /bin/bash
DOCKER_CONTAINER_ID=$(docker ps --last 4 | grep $CONTAINER_DISTRO | awk '{print $1}')

docker exec --privileged -ti $DOCKER_CONTAINER_ID apt-get update
docker exec --privileged -ti $DOCKER_CONTAINER_ID apt-get -y install dh-exec cmake \
 gettext git-core libgtk-3-dev libgl1-mesa-dev libglu1-mesa-dev libgtk2.0-dev \
 zlib1g-dev libjpeg-dev libpng-dev libtiff5-dev libsm-dev autotools-dev autoconf libexpat1-dev libxt-dev \
 libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libwebkit2gtk-4.0-dev libnotify-dev wget doxygen graphviz

GDK_PIX_VER="2.40.0+dfsg-5"
GTK_VER="3.24.23-2"
PRG_REPO=bbn-projects/bbn-repo/deb/raspbian/pool/buster
PKG_SRC=https://dl.cloudsmith.io/public/${PRG_REPO}/main

docker exec --privileged -ti $DOCKER_CONTAINER_ID /bin/bash -xec \
   "wget ${PKG_SRC}/l/li/libgdk-pixbuf2.0-bin_${GDK_PIX_VER}_${PKG_ARCH}.deb;
    wget ${PKG_SRC}/l/li/libgdk-pixbuf2.0-common_${GDK_PIX_VER}_all.deb;
    wget ${PKG_SRC}/l/li/libgdk-pixbuf2.0-0_${GDK_PIX_VER}_${PKG_ARCH}.deb;
    wget ${PKG_SRC}/g/gi/gir1.2-gdkpixbuf-2.0_${GDK_PIX_VER}_${PKG_ARCH}.deb;
    wget ${PKG_SRC}/l/li/libgdk-pixbuf2.0-dev_${GDK_PIX_VER}_${PKG_ARCH}.deb;
    wget ${PKG_SRC}/l/li/libgtk-3-common_${GTK_VER}_all.deb;
    wget ${PKG_SRC}/l/li/libgtk-3-0_${GTK_VER}_${PKG_ARCH}.deb;
    wget ${PKG_SRC}/g/gi/gir1.2-gtk-3.0_${GTK_VER}_${PKG_ARCH}.deb;
    wget ${PKG_SRC}/l/li/libgtk-3-dev_${GTK_VER}_${PKG_ARCH}.deb;
    dpkg -i libgdk-pixbuf2.0-bin_${GDK_PIX_VER}_${PKG_ARCH}.deb;
    dpkg -i libgdk-pixbuf2.0-common_${GDK_PIX_VER}_all.deb;
    dpkg -i libgdk-pixbuf2.0-0_${GDK_PIX_VER}_${PKG_ARCH}.deb;
    dpkg -i gir1.2-gdkpixbuf-2.0_${GDK_PIX_VER}_${PKG_ARCH}.deb;
    dpkg -i libgdk-pixbuf2.0-dev_${GDK_PIX_VER}_${PKG_ARCH}.deb;
    dpkg -i libgtk-3-common_${GTK_VER}_all.deb;
    dpkg -i libgtk-3-0_${GTK_VER}_${PKG_ARCH}.deb;
    dpkg -i gir1.2-gtk-3.0_${GTK_VER}_${PKG_ARCH}.deb;
    dpkg -i libgtk-3-dev_${GTK_VER}_${PKG_ARCH}.deb"

docker exec -ti $DOCKER_CONTAINER_ID /bin/bash -xec \
    "cd ci-source; dpkg-buildpackage -b -uc -us; mkdir dist; mv ../*.deb dist; chmod -R a+rw dist"

find dist -name \*.deb

echo "Stopping"
docker ps -a
docker stop $DOCKER_CONTAINER_ID
docker rm -v $DOCKER_CONTAINER_ID

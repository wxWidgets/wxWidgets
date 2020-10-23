#!/usr/bin/env bash

EXT=$1
REPO=$2
DISTRO=$3

for pkg_file in dist/*.$EXT; do
  cloudsmith push deb $REPO/$DISTRO $pkg_file
done
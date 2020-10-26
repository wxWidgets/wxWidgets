#!/usr/bin/env bash

EXT=$1
REPO=$2
DISTRO=$3

for pkg_file in dist/*.$EXT; do
  cloudsmith push $EXT $REPO/$DISTRO $pkg_file
  RESULT=$?
  if [ $RESULT -eq 144 ]; then
     echo "skipping already deployed $pkg_file"
  fi
done
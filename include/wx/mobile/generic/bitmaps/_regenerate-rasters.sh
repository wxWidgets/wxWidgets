#!/bin/sh

#
# Generates raster (PNG, XPM) images out of SVG vectors
# Requires:
#  * ImageMagick <http://www.imagemagick.org/> built with SVG (librsvg) support
#  * bin2c <http://wiki.wxwidgets.org/Embedding_PNG_Images#bin2c>
#
# Usage: ./_regenerate-rasters.sh
#
# FIXME add 'const' to generated XPMs
#

IM_CONVERT="/usr/bin/env convert"
IM_OPTIONS=" -background None"
BIN2C="/usr/bin/bin2c"

for i in *.svg; do
	echo "Creating PNG raster for '$i'..."
	$IM_CONVERT $IM_OPTIONS $i ${i%.*}.png
	echo "Creating XPM (INC) raster for '$i'..."
	$BIN2C -c ${i%.*}.png ${i%.*}.inc
done

echo "Done."

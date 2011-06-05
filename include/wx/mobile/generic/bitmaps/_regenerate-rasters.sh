#!/bin/sh

#
# Generates raster (PNG, XPM) images out of SVG vectors
# Requires ImageMagick (http://www.imagemagick.org/) built with SVG (librsvg) support
#
# Usage: ./_regenerate-rasters.sh
#

IM_CONVERT="/usr/bin/env convert"
IM_OPTIONS=" -background None"

for i in *.svg; do
	echo "Creating PNG raster for '$i'..."
	$IM_CONVERT $IM_OPTIONS $i ${i%.*}.png
	echo "Creating XPM (INC) raster for '$i'..."
	$IM_CONVERT $IM_OPTIONS ${i%.*}.png ${i%.*}_png.xpm
	mv ${i%.*}_png.xpm ${i%.*}.inc
done

echo "Done."

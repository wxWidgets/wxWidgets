#!/bin/sh

doxygen

# this image is not automatically copied by Doxygen because it's not
# used in doxygen documentation but only in our html footer...
cp images/powered-by-wxwidgets.png out/html

REM $Id$

REM This bash script regenerates the HTML doxygen version of the
REM wxWidgets manual and adjusts the doxygen log to make it more
REM readable.

doxygen

REM this image is not automatically copied by Doxygen because it's not
REM used in doxygen documentation but only in our html footer...
copy images/powered-by-wxwidgets.png out/html

REM this CSS is automatically copied by Doxygen because it's
REM included by our custom html header...
copy wxwidgets.css out/html

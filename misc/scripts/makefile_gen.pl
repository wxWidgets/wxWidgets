#!/usr/bin/env perl

# Last Modified: Sun 07 Aug 2011 01:16:59 PM CDT 
# This script generates Makefile of samples supporting multiple make jobs, i.e. make -j8. It
# shortens the make time when you need to frequently re-make samples to test wxWidgets.
# Usage:
#   makefile_gen <makefile_name>

use strict;
use warnings;
use autodie;

my @SAMPLES_SUBDIRS=qw(animate artprov aui calendar caret clipboard collpane combo config console
controls dataview debugrpt dialogs dialup display dnd docview dragimag drawing erase event except
exec font fswatcher grid help htlbox html html/about html/help html/helpview html/printing html/test
html/virtual html/widget html/zip image internat ipc joytest keyboard layout listctrl mdi
mediaplayer menu minimal notebook opengl opengl/cube opengl/isosurf opengl/penguin popup printing
propgrid render ribbon richtext sashtest scroll shaped sockets sound splash splitter statbar stc svg
taborder taskbar text thread toolbar treectrl typetest uiaction validate vscroll widgets wizard
wrapsizer xrc);

open my $OUT, '>', $ARGV[0];

printf $OUT "SAMPLES_SUBDIRS=";
foreach (@SAMPLES_SUBDIRS) {
    printf $OUT "$_ ";
}
printf $OUT "\n";

printf $OUT "all: ";
foreach (@SAMPLES_SUBDIRS) {
    printf $OUT "$_ ";
}
printf $OUT "\n";

foreach (@SAMPLES_SUBDIRS) {
    printf $OUT "$_:\n";
    printf $OUT "\tcd $_ && \$(MAKE)\n\n";
}

printf $OUT "clean:\n";
printf $OUT "\t\@for d in \$(SAMPLES_SUBDIRS); do (cd \$\$d && \$(MAKE) clean); done\n";

close $OUT;

#
# Makefile : Builds wxWindows samples for Unix.
#

SAMPLES_SUBDIRS=animate archive artprov aui calendar caret clipboard collpane combo config console dataview debugrpt dialogs dialup display dnd docview dragimag drawing erase event except exec font fswatcher grid help htlbox html html/about html/help html/helpview html/printing html/test html/virtual html/widget html/zip image internat ipc joytest keyboard layout listctrl mdi mediaplayer menu minimal notebook opengl opengl/cube opengl/isosurf opengl/penguin opengl/pyramid popup power preferences printing propgrid render ribbon richtext sashtest scroll secretstore shaped sockets sound splash splitter statbar stc svg taborder taskbar text thread toolbar treectrl treelist typetest uiaction validate vscroll webview widgets wizard wrapsizer xrc 

ALL_TARGETS := all clean distclean

$(ALL_TARGETS): $(SAMPLES_SUBDIRS)

# The use of both MAKECMDGOALS and .TARGETS here allows the same makefile to be
# used both with GNU make and BSD make: only one of these variables will be
# defined for the particular make program flavour.
$(SAMPLES_SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS) $(.TARGETS)

.PHONY: $(ALL_TARGETS) $(SAMPLES_SUBDIRS)

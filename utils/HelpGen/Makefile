##############################################################################
# Name:        Makefile
# Purpose:     Unix makefile for helpgen project
# Author:      Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
# Modified by:
# Created:     06/01/99
# RCS-ID:      $Id$
# Copyright:   (c) 1999 VZ
# Licence:     GPL
##############################################################################

# vi: set tw=0 noet ts=8 sw=8:

.SUFFIXES: .cpp

##############################################################################
# variables
##############################################################################
PROG = HelpGen
SRCS = src/cjparser.cpp\
	src/markup.cpp\
	src/scriptbinder.cpp\
	src/srcparser.cpp\
	src/ifcontext.cpp\
	src/HelpGen.cpp\
	src/sourcepainter.cpp\
	src/wx/string.cpp\
	src/wx_extra_imps.cpp\
	src/wx/object.cpp\
	src/wx/hash.cpp\
	src/wx/log.cpp\
	src/wx/file.cpp\
	src/wx/dynarray.cpp\
	src/wx/list.cpp

OBJS = obj/cjparser.o\
	obj/markup.o\
	obj/scriptbinder.o\
	obj/srcparser.o\
	obj/ifcontext.o\
	obj/HelpGen.o\
	obj/sourcepainter.o\
	obj/wx/string.o\
	obj/wx_extra_imps.o\
	obj/wx/object.o\
	obj/wx/hash.o\
	obj/wx/log.o\
	obj/wx/file.o\
	obj/wx/dynarray.o\
	obj/wx/list.o

# tested with egcs 1.1.1 (add more compilers here)
CC = g++

DEFINES = -DwxUSE_NOGUI=1 -D__UNIX__ -D__WXGTK__ -DHAVE_VSNPRINTF
# this might not work with some versions of gcc - try just -g then
DEBUG = -ggdb -D__WXDEBUG__
INCLUDE = -I./include
CPPFLAGS  = $(INCLUDE) $(DEFINES) $(DEBUG) -c -Wall

##############################################################################
# rules
##############################################################################

# building the program
obj/%.o: src/%.cpp
	$(CC) $(CPPFLAGS) -o $@ $<
	
all: $(PROG)

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(DEBUG) -Wall $(OBJS)
	
# create the links
links: include/wx src/wx
	ln -sf ../../include/wx include/wx
	ln -sf ../../src/common src/wx

# this requires exuberant ctags version 2.x or later
ctags: tags
	ctags -i mCF --lang=c++ include/*.h src/*.cpp

depend:
	makedepend $(INCLUDE) $(DEFINES) -I/usr/include/g++ -I/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include $(SRCS)

clean:
	rm -f $(OBJS) $(PROG) core

# DO NOT DELETE

src/cjparser.o: ./include/wx/wxprec.h ./include/wx/wx.h
src/cjparser.o: ./include/wx/wx/setup.h ./include/wx/wx/wx/gtk/setup.h
src/cjparser.o: ./include/wx/wx/defs.h
src/cjparser.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/cjparser.o: ./include/wx/wx/wx/version.h ./include/wx/wx/string.h
src/cjparser.o: /usr/include/string.h /usr/include/features.h
src/cjparser.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/cjparser.o: /usr/include/stdio.h /usr/include/libio.h
src/cjparser.o: /usr/include/_G_config.h /usr/include/gnu/types.h
src/cjparser.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stdarg.h
src/cjparser.o: /usr/include/stdio_lim.h
src/cjparser.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/limits.h
src/cjparser.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/syslimits.h
src/cjparser.o: /usr/include/stdlib.h /usr/include/sys/types.h
src/cjparser.o: /usr/include/time.h /usr/include/endian.h
src/cjparser.o: /usr/include/bytesex.h /usr/include/sys/select.h
src/cjparser.o: /usr/include/selectbits.h /usr/include/alloca.h
src/cjparser.o: ./include/wx/wx/wx/debug.h /usr/include/assert.h
src/cjparser.o: /usr/include/g++/iostream.h /usr/include/g++/streambuf.h
src/cjparser.o: ./include/wx/wx/log.h ./include/wx/wx/window.h
src/cjparser.o: ./include/wx/wx/wx/gtk/window.h ./include/wx/object.h
src/cjparser.o: ./include/wx/wx/memory.h ./include/wx/list.h
src/cjparser.o: ./include/wx/event.h ./include/wx/wx/gdicmn.h
src/cjparser.o: ./include/wx/wx/wx/hash.h ./include/wx/wx/wx/colour.h
src/cjparser.o: ./include/wx/wx/wx/wx/gtk/colour.h ./include/wx/gdiobj.h
src/cjparser.o: ./include/wx/wx/gtk/gdiobj.h ./include/wx/palette.h
src/cjparser.o: ./include/wx/wx/gtk/palette.h ./include/wx/validate.h
src/cjparser.o: ./include/wx/cursor.h ./include/wx/wx/gtk/cursor.h
src/cjparser.o: ./include/wx/font.h ./include/wx/wx/gtk/font.h
src/cjparser.o: ./include/wx/region.h ./include/wx/wx/gtk/region.h
src/cjparser.o: ./include/wx/accel.h ./include/wx/wx/gtk/accel.h
src/cjparser.o: ./include/wx/wx/panel.h ./include/wx/wx/wx/generic/panelg.h
src/cjparser.o: ./include/wx/wx/frame.h ./include/wx/wx/wx/gtk/frame.h
src/cjparser.o: ./include/wx/icon.h ./include/wx/wx/dc.h
src/cjparser.o: ./include/wx/wx/wx/gtk/dc.h ./include/wx/pen.h
src/cjparser.o: ./include/wx/wx/gtk/pen.h ./include/wx/brush.h
src/cjparser.o: ./include/wx/wx/gtk/brush.h ./include/wx/bitmap.h
src/cjparser.o: ./include/wx/wx/gtk/bitmap.h ./include/wx/wx/dcclient.h
src/cjparser.o: ./include/wx/wx/wx/gtk/dcclient.h ./include/wx/wx/dcmemory.h
src/cjparser.o: ./include/wx/wx/wx/gtk/dcmemory.h ./include/wx/wx/dcprint.h
src/cjparser.o: ./include/wx/wx/dcscreen.h ./include/wx/wx/wx/gtk/dcscreen.h
src/cjparser.o: ./include/wx/wx/button.h ./include/wx/wx/wx/gtk/button.h
src/cjparser.o: ./include/wx/control.h ./include/wx/wx/gtk/control.h
src/cjparser.o: ./include/wx/wx/bmpbuttn.h ./include/wx/wx/wx/gtk/bmpbuttn.h
src/cjparser.o: ./include/wx/wx/checkbox.h ./include/wx/wx/wx/gtk/checkbox.h
src/cjparser.o: ./include/wx/wx/choice.h ./include/wx/wx/wx/gtk/choice.h
src/cjparser.o: ./include/wx/wx/scrolbar.h ./include/wx/wx/wx/gtk/scrolbar.h
src/cjparser.o: ./include/wx/wx/stattext.h ./include/wx/wx/wx/gtk/stattext.h
src/cjparser.o: ./include/wx/wx/statbmp.h ./include/wx/wx/wx/gtk/statbmp.h
src/cjparser.o: ./include/wx/wx/statbox.h ./include/wx/wx/wx/gtk/statbox.h
src/cjparser.o: ./include/wx/wx/listbox.h ./include/wx/wx/wx/gtk/listbox.h
src/cjparser.o: ./include/wx/wx/radiobox.h ./include/wx/wx/wx/gtk/radiobox.h
src/cjparser.o: ./include/wx/wx/radiobut.h ./include/wx/wx/wx/gtk/radiobut.h
src/cjparser.o: ./include/wx/wx/textctrl.h ./include/wx/wx/wx/gtk/textctrl.h
src/cjparser.o: ./include/wx/wx/slider.h ./include/wx/wx/wx/gtk/slider.h
src/cjparser.o: ./include/wx/wx/gauge.h ./include/wx/wx/wx/gtk/gauge.h
src/cjparser.o: ./include/wx/wx/combobox.h ./include/wx/wx/wx/gtk/combobox.h
src/cjparser.o: ./include/wx/wx/menu.h ./include/wx/wx/wx/gtk/menu.h
src/cjparser.o: ./include/wx/menuitem.h ./include/wx/wx/gtk/menuitem.h
src/cjparser.o: ./include/wx/wx/app.h ./include/wx/wx/wx/gtk/app.h
src/cjparser.o: ./include/wx/wx/dialog.h ./include/wx/wx/wx/gtk/dialog.h
src/cjparser.o: ./include/wx/wx/timer.h ./include/wx/wx/wx/gtk/timer.h
src/cjparser.o: ./include/wx/wx/utils.h ./include/wx/wx/wx/filefn.h
src/cjparser.o: ./include/wx/wx/wx/process.h ./include/wx/wx/settings.h
src/cjparser.o: ./include/wx/wx/wx/gtk/settings.h ./include/wx/wx/layout.h
src/cjparser.o: ./include/wx/wx/mdi.h ./include/wx/wx/wx/gtk/mdi.h
src/cjparser.o: ./include/wx/toolbar.h ./include/wx/wx/gtk/tbargtk.h
src/cjparser.o: ./include/wx/wx/scrolwin.h
src/cjparser.o: ./include/wx/wx/wx/generic/scrolwin.h
src/cjparser.o: ./include/wx/wx/statusbr.h
src/cjparser.o: ./include/wx/wx/wx/generic/statusbr.h
src/cjparser.o: ./include/wx/wx/msgdlg.h ./include/wx/wx/wx/generic/msgdlgg.h
src/cjparser.o: ./include/wx/wx/choicdlg.h
src/cjparser.o: ./include/wx/wx/wx/generic/choicdgg.h
src/cjparser.o: ./include/wx/wx/textdlg.h
src/cjparser.o: ./include/wx/wx/wx/generic/textdlgg.h
src/cjparser.o: ./include/wx/wx/filedlg.h ./include/wx/wx/wx/gtk/filedlg.h
src/cjparser.o: ./include/wx/wx/dirdlg.h ./include/wx/wx/wx/generic/dirdlgg.h
src/cjparser.o: ./include/wx/treectrl.h ./include/wx/wx/generic/treectrl.h
src/cjparser.o: ./include/wx/wx/cmndata.h ./include/wx/wx/intl.h
src/cjparser.o: ./include/wx/wx/valtext.h ./include/cjparser.h
src/cjparser.o: ./include/srcparser.h ./include/markup.h
src/cjparser.o: /usr/include/memory.h
src/markup.o: ./include/wx/wxprec.h ./include/wx/wx.h ./include/wx/wx/setup.h
src/markup.o: ./include/wx/wx/wx/gtk/setup.h ./include/wx/wx/defs.h
src/markup.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/markup.o: ./include/wx/wx/wx/version.h ./include/wx/wx/string.h
src/markup.o: /usr/include/string.h /usr/include/features.h
src/markup.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/markup.o: /usr/include/stdio.h /usr/include/libio.h
src/markup.o: /usr/include/_G_config.h /usr/include/gnu/types.h
src/markup.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stdarg.h
src/markup.o: /usr/include/stdio_lim.h
src/markup.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/limits.h
src/markup.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/syslimits.h
src/markup.o: /usr/include/stdlib.h /usr/include/sys/types.h
src/markup.o: /usr/include/time.h /usr/include/endian.h
src/markup.o: /usr/include/bytesex.h /usr/include/sys/select.h
src/markup.o: /usr/include/selectbits.h /usr/include/alloca.h
src/markup.o: ./include/wx/wx/wx/debug.h /usr/include/assert.h
src/markup.o: /usr/include/g++/iostream.h /usr/include/g++/streambuf.h
src/markup.o: ./include/wx/wx/log.h ./include/wx/wx/window.h
src/markup.o: ./include/wx/wx/wx/gtk/window.h ./include/wx/object.h
src/markup.o: ./include/wx/wx/memory.h ./include/wx/list.h
src/markup.o: ./include/wx/event.h ./include/wx/wx/gdicmn.h
src/markup.o: ./include/wx/wx/wx/hash.h ./include/wx/wx/wx/colour.h
src/markup.o: ./include/wx/wx/wx/wx/gtk/colour.h ./include/wx/gdiobj.h
src/markup.o: ./include/wx/wx/gtk/gdiobj.h ./include/wx/palette.h
src/markup.o: ./include/wx/wx/gtk/palette.h ./include/wx/validate.h
src/markup.o: ./include/wx/cursor.h ./include/wx/wx/gtk/cursor.h
src/markup.o: ./include/wx/font.h ./include/wx/wx/gtk/font.h
src/markup.o: ./include/wx/region.h ./include/wx/wx/gtk/region.h
src/markup.o: ./include/wx/accel.h ./include/wx/wx/gtk/accel.h
src/markup.o: ./include/wx/wx/panel.h ./include/wx/wx/wx/generic/panelg.h
src/markup.o: ./include/wx/wx/frame.h ./include/wx/wx/wx/gtk/frame.h
src/markup.o: ./include/wx/icon.h ./include/wx/wx/dc.h
src/markup.o: ./include/wx/wx/wx/gtk/dc.h ./include/wx/pen.h
src/markup.o: ./include/wx/wx/gtk/pen.h ./include/wx/brush.h
src/markup.o: ./include/wx/wx/gtk/brush.h ./include/wx/bitmap.h
src/markup.o: ./include/wx/wx/gtk/bitmap.h ./include/wx/wx/dcclient.h
src/markup.o: ./include/wx/wx/wx/gtk/dcclient.h ./include/wx/wx/dcmemory.h
src/markup.o: ./include/wx/wx/wx/gtk/dcmemory.h ./include/wx/wx/dcprint.h
src/markup.o: ./include/wx/wx/dcscreen.h ./include/wx/wx/wx/gtk/dcscreen.h
src/markup.o: ./include/wx/wx/button.h ./include/wx/wx/wx/gtk/button.h
src/markup.o: ./include/wx/control.h ./include/wx/wx/gtk/control.h
src/markup.o: ./include/wx/wx/bmpbuttn.h ./include/wx/wx/wx/gtk/bmpbuttn.h
src/markup.o: ./include/wx/wx/checkbox.h ./include/wx/wx/wx/gtk/checkbox.h
src/markup.o: ./include/wx/wx/choice.h ./include/wx/wx/wx/gtk/choice.h
src/markup.o: ./include/wx/wx/scrolbar.h ./include/wx/wx/wx/gtk/scrolbar.h
src/markup.o: ./include/wx/wx/stattext.h ./include/wx/wx/wx/gtk/stattext.h
src/markup.o: ./include/wx/wx/statbmp.h ./include/wx/wx/wx/gtk/statbmp.h
src/markup.o: ./include/wx/wx/statbox.h ./include/wx/wx/wx/gtk/statbox.h
src/markup.o: ./include/wx/wx/listbox.h ./include/wx/wx/wx/gtk/listbox.h
src/markup.o: ./include/wx/wx/radiobox.h ./include/wx/wx/wx/gtk/radiobox.h
src/markup.o: ./include/wx/wx/radiobut.h ./include/wx/wx/wx/gtk/radiobut.h
src/markup.o: ./include/wx/wx/textctrl.h ./include/wx/wx/wx/gtk/textctrl.h
src/markup.o: ./include/wx/wx/slider.h ./include/wx/wx/wx/gtk/slider.h
src/markup.o: ./include/wx/wx/gauge.h ./include/wx/wx/wx/gtk/gauge.h
src/markup.o: ./include/wx/wx/combobox.h ./include/wx/wx/wx/gtk/combobox.h
src/markup.o: ./include/wx/wx/menu.h ./include/wx/wx/wx/gtk/menu.h
src/markup.o: ./include/wx/menuitem.h ./include/wx/wx/gtk/menuitem.h
src/markup.o: ./include/wx/wx/app.h ./include/wx/wx/wx/gtk/app.h
src/markup.o: ./include/wx/wx/dialog.h ./include/wx/wx/wx/gtk/dialog.h
src/markup.o: ./include/wx/wx/timer.h ./include/wx/wx/wx/gtk/timer.h
src/markup.o: ./include/wx/wx/utils.h ./include/wx/wx/wx/filefn.h
src/markup.o: ./include/wx/wx/wx/process.h ./include/wx/wx/settings.h
src/markup.o: ./include/wx/wx/wx/gtk/settings.h ./include/wx/wx/layout.h
src/markup.o: ./include/wx/wx/mdi.h ./include/wx/wx/wx/gtk/mdi.h
src/markup.o: ./include/wx/toolbar.h ./include/wx/wx/gtk/tbargtk.h
src/markup.o: ./include/wx/wx/scrolwin.h
src/markup.o: ./include/wx/wx/wx/generic/scrolwin.h
src/markup.o: ./include/wx/wx/statusbr.h
src/markup.o: ./include/wx/wx/wx/generic/statusbr.h ./include/wx/wx/msgdlg.h
src/markup.o: ./include/wx/wx/wx/generic/msgdlgg.h ./include/wx/wx/choicdlg.h
src/markup.o: ./include/wx/wx/wx/generic/choicdgg.h ./include/wx/wx/textdlg.h
src/markup.o: ./include/wx/wx/wx/generic/textdlgg.h ./include/wx/wx/filedlg.h
src/markup.o: ./include/wx/wx/wx/gtk/filedlg.h ./include/wx/wx/dirdlg.h
src/markup.o: ./include/wx/wx/wx/generic/dirdlgg.h ./include/wx/treectrl.h
src/markup.o: ./include/wx/wx/generic/treectrl.h ./include/wx/wx/cmndata.h
src/markup.o: ./include/wx/wx/intl.h ./include/wx/wx/valtext.h
src/markup.o: ./include/markup.h
src/scriptbinder.o: ./include/wx/wxprec.h ./include/wx/wx.h
src/scriptbinder.o: ./include/wx/wx/setup.h ./include/wx/wx/wx/gtk/setup.h
src/scriptbinder.o: ./include/wx/wx/defs.h
src/scriptbinder.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/scriptbinder.o: ./include/wx/wx/wx/version.h ./include/wx/wx/string.h
src/scriptbinder.o: /usr/include/string.h /usr/include/features.h
src/scriptbinder.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/scriptbinder.o: /usr/include/stdio.h /usr/include/libio.h
src/scriptbinder.o: /usr/include/_G_config.h /usr/include/gnu/types.h
src/scriptbinder.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stdarg.h
src/scriptbinder.o: /usr/include/stdio_lim.h
src/scriptbinder.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/limits.h
src/scriptbinder.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/syslimits.h
src/scriptbinder.o: /usr/include/stdlib.h /usr/include/sys/types.h
src/scriptbinder.o: /usr/include/time.h /usr/include/endian.h
src/scriptbinder.o: /usr/include/bytesex.h /usr/include/sys/select.h
src/scriptbinder.o: /usr/include/selectbits.h /usr/include/alloca.h
src/scriptbinder.o: ./include/wx/wx/wx/debug.h /usr/include/assert.h
src/scriptbinder.o: /usr/include/g++/iostream.h /usr/include/g++/streambuf.h
src/scriptbinder.o: ./include/wx/wx/log.h ./include/wx/wx/window.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/window.h ./include/wx/object.h
src/scriptbinder.o: ./include/wx/wx/memory.h ./include/wx/list.h
src/scriptbinder.o: ./include/wx/event.h ./include/wx/wx/gdicmn.h
src/scriptbinder.o: ./include/wx/wx/wx/hash.h ./include/wx/wx/wx/colour.h
src/scriptbinder.o: ./include/wx/wx/wx/wx/gtk/colour.h ./include/wx/gdiobj.h
src/scriptbinder.o: ./include/wx/wx/gtk/gdiobj.h ./include/wx/palette.h
src/scriptbinder.o: ./include/wx/wx/gtk/palette.h ./include/wx/validate.h
src/scriptbinder.o: ./include/wx/cursor.h ./include/wx/wx/gtk/cursor.h
src/scriptbinder.o: ./include/wx/font.h ./include/wx/wx/gtk/font.h
src/scriptbinder.o: ./include/wx/region.h ./include/wx/wx/gtk/region.h
src/scriptbinder.o: ./include/wx/accel.h ./include/wx/wx/gtk/accel.h
src/scriptbinder.o: ./include/wx/wx/panel.h
src/scriptbinder.o: ./include/wx/wx/wx/generic/panelg.h
src/scriptbinder.o: ./include/wx/wx/frame.h ./include/wx/wx/wx/gtk/frame.h
src/scriptbinder.o: ./include/wx/icon.h ./include/wx/wx/dc.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/dc.h ./include/wx/pen.h
src/scriptbinder.o: ./include/wx/wx/gtk/pen.h ./include/wx/brush.h
src/scriptbinder.o: ./include/wx/wx/gtk/brush.h ./include/wx/bitmap.h
src/scriptbinder.o: ./include/wx/wx/gtk/bitmap.h ./include/wx/wx/dcclient.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/dcclient.h
src/scriptbinder.o: ./include/wx/wx/dcmemory.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/dcmemory.h
src/scriptbinder.o: ./include/wx/wx/dcprint.h ./include/wx/wx/dcscreen.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/dcscreen.h
src/scriptbinder.o: ./include/wx/wx/button.h ./include/wx/wx/wx/gtk/button.h
src/scriptbinder.o: ./include/wx/control.h ./include/wx/wx/gtk/control.h
src/scriptbinder.o: ./include/wx/wx/bmpbuttn.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/bmpbuttn.h
src/scriptbinder.o: ./include/wx/wx/checkbox.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/checkbox.h
src/scriptbinder.o: ./include/wx/wx/choice.h ./include/wx/wx/wx/gtk/choice.h
src/scriptbinder.o: ./include/wx/wx/scrolbar.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/scrolbar.h
src/scriptbinder.o: ./include/wx/wx/stattext.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/stattext.h
src/scriptbinder.o: ./include/wx/wx/statbmp.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/statbmp.h
src/scriptbinder.o: ./include/wx/wx/statbox.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/statbox.h
src/scriptbinder.o: ./include/wx/wx/listbox.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/listbox.h
src/scriptbinder.o: ./include/wx/wx/radiobox.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/radiobox.h
src/scriptbinder.o: ./include/wx/wx/radiobut.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/radiobut.h
src/scriptbinder.o: ./include/wx/wx/textctrl.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/textctrl.h
src/scriptbinder.o: ./include/wx/wx/slider.h ./include/wx/wx/wx/gtk/slider.h
src/scriptbinder.o: ./include/wx/wx/gauge.h ./include/wx/wx/wx/gtk/gauge.h
src/scriptbinder.o: ./include/wx/wx/combobox.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/combobox.h ./include/wx/wx/menu.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/menu.h ./include/wx/menuitem.h
src/scriptbinder.o: ./include/wx/wx/gtk/menuitem.h ./include/wx/wx/app.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/app.h ./include/wx/wx/dialog.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/dialog.h ./include/wx/wx/timer.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/timer.h ./include/wx/wx/utils.h
src/scriptbinder.o: ./include/wx/wx/wx/filefn.h ./include/wx/wx/wx/process.h
src/scriptbinder.o: ./include/wx/wx/settings.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/settings.h
src/scriptbinder.o: ./include/wx/wx/layout.h ./include/wx/wx/mdi.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/mdi.h ./include/wx/toolbar.h
src/scriptbinder.o: ./include/wx/wx/gtk/tbargtk.h ./include/wx/wx/scrolwin.h
src/scriptbinder.o: ./include/wx/wx/wx/generic/scrolwin.h
src/scriptbinder.o: ./include/wx/wx/statusbr.h
src/scriptbinder.o: ./include/wx/wx/wx/generic/statusbr.h
src/scriptbinder.o: ./include/wx/wx/msgdlg.h
src/scriptbinder.o: ./include/wx/wx/wx/generic/msgdlgg.h
src/scriptbinder.o: ./include/wx/wx/choicdlg.h
src/scriptbinder.o: ./include/wx/wx/wx/generic/choicdgg.h
src/scriptbinder.o: ./include/wx/wx/textdlg.h
src/scriptbinder.o: ./include/wx/wx/wx/generic/textdlgg.h
src/scriptbinder.o: ./include/wx/wx/filedlg.h
src/scriptbinder.o: ./include/wx/wx/wx/gtk/filedlg.h ./include/wx/wx/dirdlg.h
src/scriptbinder.o: ./include/wx/wx/wx/generic/dirdlgg.h
src/scriptbinder.o: ./include/wx/treectrl.h
src/scriptbinder.o: ./include/wx/wx/generic/treectrl.h
src/scriptbinder.o: ./include/wx/wx/cmndata.h ./include/wx/wx/intl.h
src/scriptbinder.o: ./include/wx/wx/valtext.h /usr/include/malloc.h
src/scriptbinder.o: /usr/include/memory.h ./include/scriptbinder.h
src/scriptbinder.o: ./include/markup.h
src/srcparser.o: ./include/wx/wxprec.h ./include/wx/wx.h
src/srcparser.o: ./include/wx/wx/setup.h ./include/wx/wx/wx/gtk/setup.h
src/srcparser.o: ./include/wx/wx/defs.h
src/srcparser.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/srcparser.o: ./include/wx/wx/wx/version.h ./include/wx/wx/string.h
src/srcparser.o: /usr/include/string.h /usr/include/features.h
src/srcparser.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/srcparser.o: /usr/include/stdio.h /usr/include/libio.h
src/srcparser.o: /usr/include/_G_config.h /usr/include/gnu/types.h
src/srcparser.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stdarg.h
src/srcparser.o: /usr/include/stdio_lim.h
src/srcparser.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/limits.h
src/srcparser.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/syslimits.h
src/srcparser.o: /usr/include/stdlib.h /usr/include/sys/types.h
src/srcparser.o: /usr/include/time.h /usr/include/endian.h
src/srcparser.o: /usr/include/bytesex.h /usr/include/sys/select.h
src/srcparser.o: /usr/include/selectbits.h /usr/include/alloca.h
src/srcparser.o: ./include/wx/wx/wx/debug.h /usr/include/assert.h
src/srcparser.o: /usr/include/g++/iostream.h /usr/include/g++/streambuf.h
src/srcparser.o: ./include/wx/wx/log.h ./include/wx/wx/window.h
src/srcparser.o: ./include/wx/wx/wx/gtk/window.h ./include/wx/object.h
src/srcparser.o: ./include/wx/wx/memory.h ./include/wx/list.h
src/srcparser.o: ./include/wx/event.h ./include/wx/wx/gdicmn.h
src/srcparser.o: ./include/wx/wx/wx/hash.h ./include/wx/wx/wx/colour.h
src/srcparser.o: ./include/wx/wx/wx/wx/gtk/colour.h ./include/wx/gdiobj.h
src/srcparser.o: ./include/wx/wx/gtk/gdiobj.h ./include/wx/palette.h
src/srcparser.o: ./include/wx/wx/gtk/palette.h ./include/wx/validate.h
src/srcparser.o: ./include/wx/cursor.h ./include/wx/wx/gtk/cursor.h
src/srcparser.o: ./include/wx/font.h ./include/wx/wx/gtk/font.h
src/srcparser.o: ./include/wx/region.h ./include/wx/wx/gtk/region.h
src/srcparser.o: ./include/wx/accel.h ./include/wx/wx/gtk/accel.h
src/srcparser.o: ./include/wx/wx/panel.h ./include/wx/wx/wx/generic/panelg.h
src/srcparser.o: ./include/wx/wx/frame.h ./include/wx/wx/wx/gtk/frame.h
src/srcparser.o: ./include/wx/icon.h ./include/wx/wx/dc.h
src/srcparser.o: ./include/wx/wx/wx/gtk/dc.h ./include/wx/pen.h
src/srcparser.o: ./include/wx/wx/gtk/pen.h ./include/wx/brush.h
src/srcparser.o: ./include/wx/wx/gtk/brush.h ./include/wx/bitmap.h
src/srcparser.o: ./include/wx/wx/gtk/bitmap.h ./include/wx/wx/dcclient.h
src/srcparser.o: ./include/wx/wx/wx/gtk/dcclient.h ./include/wx/wx/dcmemory.h
src/srcparser.o: ./include/wx/wx/wx/gtk/dcmemory.h ./include/wx/wx/dcprint.h
src/srcparser.o: ./include/wx/wx/dcscreen.h ./include/wx/wx/wx/gtk/dcscreen.h
src/srcparser.o: ./include/wx/wx/button.h ./include/wx/wx/wx/gtk/button.h
src/srcparser.o: ./include/wx/control.h ./include/wx/wx/gtk/control.h
src/srcparser.o: ./include/wx/wx/bmpbuttn.h ./include/wx/wx/wx/gtk/bmpbuttn.h
src/srcparser.o: ./include/wx/wx/checkbox.h ./include/wx/wx/wx/gtk/checkbox.h
src/srcparser.o: ./include/wx/wx/choice.h ./include/wx/wx/wx/gtk/choice.h
src/srcparser.o: ./include/wx/wx/scrolbar.h ./include/wx/wx/wx/gtk/scrolbar.h
src/srcparser.o: ./include/wx/wx/stattext.h ./include/wx/wx/wx/gtk/stattext.h
src/srcparser.o: ./include/wx/wx/statbmp.h ./include/wx/wx/wx/gtk/statbmp.h
src/srcparser.o: ./include/wx/wx/statbox.h ./include/wx/wx/wx/gtk/statbox.h
src/srcparser.o: ./include/wx/wx/listbox.h ./include/wx/wx/wx/gtk/listbox.h
src/srcparser.o: ./include/wx/wx/radiobox.h ./include/wx/wx/wx/gtk/radiobox.h
src/srcparser.o: ./include/wx/wx/radiobut.h ./include/wx/wx/wx/gtk/radiobut.h
src/srcparser.o: ./include/wx/wx/textctrl.h ./include/wx/wx/wx/gtk/textctrl.h
src/srcparser.o: ./include/wx/wx/slider.h ./include/wx/wx/wx/gtk/slider.h
src/srcparser.o: ./include/wx/wx/gauge.h ./include/wx/wx/wx/gtk/gauge.h
src/srcparser.o: ./include/wx/wx/combobox.h ./include/wx/wx/wx/gtk/combobox.h
src/srcparser.o: ./include/wx/wx/menu.h ./include/wx/wx/wx/gtk/menu.h
src/srcparser.o: ./include/wx/menuitem.h ./include/wx/wx/gtk/menuitem.h
src/srcparser.o: ./include/wx/wx/app.h ./include/wx/wx/wx/gtk/app.h
src/srcparser.o: ./include/wx/wx/dialog.h ./include/wx/wx/wx/gtk/dialog.h
src/srcparser.o: ./include/wx/wx/timer.h ./include/wx/wx/wx/gtk/timer.h
src/srcparser.o: ./include/wx/wx/utils.h ./include/wx/wx/wx/filefn.h
src/srcparser.o: ./include/wx/wx/wx/process.h ./include/wx/wx/settings.h
src/srcparser.o: ./include/wx/wx/wx/gtk/settings.h ./include/wx/wx/layout.h
src/srcparser.o: ./include/wx/wx/mdi.h ./include/wx/wx/wx/gtk/mdi.h
src/srcparser.o: ./include/wx/toolbar.h ./include/wx/wx/gtk/tbargtk.h
src/srcparser.o: ./include/wx/wx/scrolwin.h
src/srcparser.o: ./include/wx/wx/wx/generic/scrolwin.h
src/srcparser.o: ./include/wx/wx/statusbr.h
src/srcparser.o: ./include/wx/wx/wx/generic/statusbr.h
src/srcparser.o: ./include/wx/wx/msgdlg.h
src/srcparser.o: ./include/wx/wx/wx/generic/msgdlgg.h
src/srcparser.o: ./include/wx/wx/choicdlg.h
src/srcparser.o: ./include/wx/wx/wx/generic/choicdgg.h
src/srcparser.o: ./include/wx/wx/textdlg.h
src/srcparser.o: ./include/wx/wx/wx/generic/textdlgg.h
src/srcparser.o: ./include/wx/wx/filedlg.h ./include/wx/wx/wx/gtk/filedlg.h
src/srcparser.o: ./include/wx/wx/dirdlg.h
src/srcparser.o: ./include/wx/wx/wx/generic/dirdlgg.h ./include/wx/treectrl.h
src/srcparser.o: ./include/wx/wx/generic/treectrl.h ./include/wx/wx/cmndata.h
src/srcparser.o: ./include/wx/wx/intl.h ./include/wx/wx/valtext.h
src/srcparser.o: /usr/include/malloc.h ./include/srcparser.h
src/srcparser.o: ./include/markup.h
src/ifcontext.o: ./include/wx/wxprec.h ./include/wx/wx.h
src/ifcontext.o: ./include/wx/wx/setup.h ./include/wx/wx/wx/gtk/setup.h
src/ifcontext.o: ./include/wx/wx/defs.h
src/ifcontext.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/ifcontext.o: ./include/wx/wx/wx/version.h ./include/wx/wx/string.h
src/ifcontext.o: /usr/include/string.h /usr/include/features.h
src/ifcontext.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/ifcontext.o: /usr/include/stdio.h /usr/include/libio.h
src/ifcontext.o: /usr/include/_G_config.h /usr/include/gnu/types.h
src/ifcontext.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stdarg.h
src/ifcontext.o: /usr/include/stdio_lim.h
src/ifcontext.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/limits.h
src/ifcontext.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/syslimits.h
src/ifcontext.o: /usr/include/stdlib.h /usr/include/sys/types.h
src/ifcontext.o: /usr/include/time.h /usr/include/endian.h
src/ifcontext.o: /usr/include/bytesex.h /usr/include/sys/select.h
src/ifcontext.o: /usr/include/selectbits.h /usr/include/alloca.h
src/ifcontext.o: ./include/wx/wx/wx/debug.h /usr/include/assert.h
src/ifcontext.o: /usr/include/g++/iostream.h /usr/include/g++/streambuf.h
src/ifcontext.o: ./include/wx/wx/log.h ./include/wx/wx/window.h
src/ifcontext.o: ./include/wx/wx/wx/gtk/window.h ./include/wx/object.h
src/ifcontext.o: ./include/wx/wx/memory.h ./include/wx/list.h
src/ifcontext.o: ./include/wx/event.h ./include/wx/wx/gdicmn.h
src/ifcontext.o: ./include/wx/wx/wx/hash.h ./include/wx/wx/wx/colour.h
src/ifcontext.o: ./include/wx/wx/wx/wx/gtk/colour.h ./include/wx/gdiobj.h
src/ifcontext.o: ./include/wx/wx/gtk/gdiobj.h ./include/wx/palette.h
src/ifcontext.o: ./include/wx/wx/gtk/palette.h ./include/wx/validate.h
src/ifcontext.o: ./include/wx/cursor.h ./include/wx/wx/gtk/cursor.h
src/ifcontext.o: ./include/wx/font.h ./include/wx/wx/gtk/font.h
src/ifcontext.o: ./include/wx/region.h ./include/wx/wx/gtk/region.h
src/ifcontext.o: ./include/wx/accel.h ./include/wx/wx/gtk/accel.h
src/ifcontext.o: ./include/wx/wx/panel.h ./include/wx/wx/wx/generic/panelg.h
src/ifcontext.o: ./include/wx/wx/frame.h ./include/wx/wx/wx/gtk/frame.h
src/ifcontext.o: ./include/wx/icon.h ./include/wx/wx/dc.h
src/ifcontext.o: ./include/wx/wx/wx/gtk/dc.h ./include/wx/pen.h
src/ifcontext.o: ./include/wx/wx/gtk/pen.h ./include/wx/brush.h
src/ifcontext.o: ./include/wx/wx/gtk/brush.h ./include/wx/bitmap.h
src/ifcontext.o: ./include/wx/wx/gtk/bitmap.h ./include/wx/wx/dcclient.h
src/ifcontext.o: ./include/wx/wx/wx/gtk/dcclient.h ./include/wx/wx/dcmemory.h
src/ifcontext.o: ./include/wx/wx/wx/gtk/dcmemory.h ./include/wx/wx/dcprint.h
src/ifcontext.o: ./include/wx/wx/dcscreen.h ./include/wx/wx/wx/gtk/dcscreen.h
src/ifcontext.o: ./include/wx/wx/button.h ./include/wx/wx/wx/gtk/button.h
src/ifcontext.o: ./include/wx/control.h ./include/wx/wx/gtk/control.h
src/ifcontext.o: ./include/wx/wx/bmpbuttn.h ./include/wx/wx/wx/gtk/bmpbuttn.h
src/ifcontext.o: ./include/wx/wx/checkbox.h ./include/wx/wx/wx/gtk/checkbox.h
src/ifcontext.o: ./include/wx/wx/choice.h ./include/wx/wx/wx/gtk/choice.h
src/ifcontext.o: ./include/wx/wx/scrolbar.h ./include/wx/wx/wx/gtk/scrolbar.h
src/ifcontext.o: ./include/wx/wx/stattext.h ./include/wx/wx/wx/gtk/stattext.h
src/ifcontext.o: ./include/wx/wx/statbmp.h ./include/wx/wx/wx/gtk/statbmp.h
src/ifcontext.o: ./include/wx/wx/statbox.h ./include/wx/wx/wx/gtk/statbox.h
src/ifcontext.o: ./include/wx/wx/listbox.h ./include/wx/wx/wx/gtk/listbox.h
src/ifcontext.o: ./include/wx/wx/radiobox.h ./include/wx/wx/wx/gtk/radiobox.h
src/ifcontext.o: ./include/wx/wx/radiobut.h ./include/wx/wx/wx/gtk/radiobut.h
src/ifcontext.o: ./include/wx/wx/textctrl.h ./include/wx/wx/wx/gtk/textctrl.h
src/ifcontext.o: ./include/wx/wx/slider.h ./include/wx/wx/wx/gtk/slider.h
src/ifcontext.o: ./include/wx/wx/gauge.h ./include/wx/wx/wx/gtk/gauge.h
src/ifcontext.o: ./include/wx/wx/combobox.h ./include/wx/wx/wx/gtk/combobox.h
src/ifcontext.o: ./include/wx/wx/menu.h ./include/wx/wx/wx/gtk/menu.h
src/ifcontext.o: ./include/wx/menuitem.h ./include/wx/wx/gtk/menuitem.h
src/ifcontext.o: ./include/wx/wx/app.h ./include/wx/wx/wx/gtk/app.h
src/ifcontext.o: ./include/wx/wx/dialog.h ./include/wx/wx/wx/gtk/dialog.h
src/ifcontext.o: ./include/wx/wx/timer.h ./include/wx/wx/wx/gtk/timer.h
src/ifcontext.o: ./include/wx/wx/utils.h ./include/wx/wx/wx/filefn.h
src/ifcontext.o: ./include/wx/wx/wx/process.h ./include/wx/wx/settings.h
src/ifcontext.o: ./include/wx/wx/wx/gtk/settings.h ./include/wx/wx/layout.h
src/ifcontext.o: ./include/wx/wx/mdi.h ./include/wx/wx/wx/gtk/mdi.h
src/ifcontext.o: ./include/wx/toolbar.h ./include/wx/wx/gtk/tbargtk.h
src/ifcontext.o: ./include/wx/wx/scrolwin.h
src/ifcontext.o: ./include/wx/wx/wx/generic/scrolwin.h
src/ifcontext.o: ./include/wx/wx/statusbr.h
src/ifcontext.o: ./include/wx/wx/wx/generic/statusbr.h
src/ifcontext.o: ./include/wx/wx/msgdlg.h
src/ifcontext.o: ./include/wx/wx/wx/generic/msgdlgg.h
src/ifcontext.o: ./include/wx/wx/choicdlg.h
src/ifcontext.o: ./include/wx/wx/wx/generic/choicdgg.h
src/ifcontext.o: ./include/wx/wx/textdlg.h
src/ifcontext.o: ./include/wx/wx/wx/generic/textdlgg.h
src/ifcontext.o: ./include/wx/wx/filedlg.h ./include/wx/wx/wx/gtk/filedlg.h
src/ifcontext.o: ./include/wx/wx/dirdlg.h
src/ifcontext.o: ./include/wx/wx/wx/generic/dirdlgg.h ./include/wx/treectrl.h
src/ifcontext.o: ./include/wx/wx/generic/treectrl.h ./include/wx/wx/cmndata.h
src/ifcontext.o: ./include/wx/wx/intl.h ./include/wx/wx/valtext.h
src/ifcontext.o: /usr/include/malloc.h ./include/ifcontext.h
src/ifcontext.o: ./include/srcparser.h ./include/markup.h
src/ifcontext.o: ./include/cjparser.h /usr/include/memory.h
src/HelpGen.o: ./include/wx/wxprec.h ./include/cjparser.h
src/HelpGen.o: ./include/srcparser.h ./include/markup.h
src/HelpGen.o: /usr/include/g++/iostream.h /usr/include/g++/streambuf.h
src/HelpGen.o: /usr/include/libio.h /usr/include/features.h
src/HelpGen.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/HelpGen.o: /usr/include/_G_config.h /usr/include/gnu/types.h
src/HelpGen.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/HelpGen.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stdarg.h
src/HelpGen.o: /usr/include/memory.h /usr/include/stdlib.h
src/HelpGen.o: /usr/include/sys/types.h /usr/include/time.h
src/HelpGen.o: /usr/include/endian.h /usr/include/bytesex.h
src/HelpGen.o: /usr/include/sys/select.h /usr/include/selectbits.h
src/HelpGen.o: /usr/include/alloca.h /usr/include/stdio.h
src/HelpGen.o: /usr/include/stdio_lim.h
src/sourcepainter.o: ./include/wx/wxprec.h ./include/wx/wx.h
src/sourcepainter.o: ./include/wx/wx/setup.h ./include/wx/wx/wx/gtk/setup.h
src/sourcepainter.o: ./include/wx/wx/defs.h
src/sourcepainter.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/sourcepainter.o: ./include/wx/wx/wx/version.h ./include/wx/wx/string.h
src/sourcepainter.o: /usr/include/string.h /usr/include/features.h
src/sourcepainter.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/sourcepainter.o: /usr/include/stdio.h /usr/include/libio.h
src/sourcepainter.o: /usr/include/_G_config.h /usr/include/gnu/types.h
src/sourcepainter.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stdarg.h
src/sourcepainter.o: /usr/include/stdio_lim.h
src/sourcepainter.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/limits.h
src/sourcepainter.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/syslimits.h
src/sourcepainter.o: /usr/include/stdlib.h /usr/include/sys/types.h
src/sourcepainter.o: /usr/include/time.h /usr/include/endian.h
src/sourcepainter.o: /usr/include/bytesex.h /usr/include/sys/select.h
src/sourcepainter.o: /usr/include/selectbits.h /usr/include/alloca.h
src/sourcepainter.o: ./include/wx/wx/wx/debug.h /usr/include/assert.h
src/sourcepainter.o: /usr/include/g++/iostream.h /usr/include/g++/streambuf.h
src/sourcepainter.o: ./include/wx/wx/log.h ./include/wx/wx/window.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/window.h ./include/wx/object.h
src/sourcepainter.o: ./include/wx/wx/memory.h ./include/wx/list.h
src/sourcepainter.o: ./include/wx/event.h ./include/wx/wx/gdicmn.h
src/sourcepainter.o: ./include/wx/wx/wx/hash.h ./include/wx/wx/wx/colour.h
src/sourcepainter.o: ./include/wx/wx/wx/wx/gtk/colour.h ./include/wx/gdiobj.h
src/sourcepainter.o: ./include/wx/wx/gtk/gdiobj.h ./include/wx/palette.h
src/sourcepainter.o: ./include/wx/wx/gtk/palette.h ./include/wx/validate.h
src/sourcepainter.o: ./include/wx/cursor.h ./include/wx/wx/gtk/cursor.h
src/sourcepainter.o: ./include/wx/font.h ./include/wx/wx/gtk/font.h
src/sourcepainter.o: ./include/wx/region.h ./include/wx/wx/gtk/region.h
src/sourcepainter.o: ./include/wx/accel.h ./include/wx/wx/gtk/accel.h
src/sourcepainter.o: ./include/wx/wx/panel.h
src/sourcepainter.o: ./include/wx/wx/wx/generic/panelg.h
src/sourcepainter.o: ./include/wx/wx/frame.h ./include/wx/wx/wx/gtk/frame.h
src/sourcepainter.o: ./include/wx/icon.h ./include/wx/wx/dc.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/dc.h ./include/wx/pen.h
src/sourcepainter.o: ./include/wx/wx/gtk/pen.h ./include/wx/brush.h
src/sourcepainter.o: ./include/wx/wx/gtk/brush.h ./include/wx/bitmap.h
src/sourcepainter.o: ./include/wx/wx/gtk/bitmap.h ./include/wx/wx/dcclient.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/dcclient.h
src/sourcepainter.o: ./include/wx/wx/dcmemory.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/dcmemory.h
src/sourcepainter.o: ./include/wx/wx/dcprint.h ./include/wx/wx/dcscreen.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/dcscreen.h
src/sourcepainter.o: ./include/wx/wx/button.h ./include/wx/wx/wx/gtk/button.h
src/sourcepainter.o: ./include/wx/control.h ./include/wx/wx/gtk/control.h
src/sourcepainter.o: ./include/wx/wx/bmpbuttn.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/bmpbuttn.h
src/sourcepainter.o: ./include/wx/wx/checkbox.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/checkbox.h
src/sourcepainter.o: ./include/wx/wx/choice.h ./include/wx/wx/wx/gtk/choice.h
src/sourcepainter.o: ./include/wx/wx/scrolbar.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/scrolbar.h
src/sourcepainter.o: ./include/wx/wx/stattext.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/stattext.h
src/sourcepainter.o: ./include/wx/wx/statbmp.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/statbmp.h
src/sourcepainter.o: ./include/wx/wx/statbox.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/statbox.h
src/sourcepainter.o: ./include/wx/wx/listbox.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/listbox.h
src/sourcepainter.o: ./include/wx/wx/radiobox.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/radiobox.h
src/sourcepainter.o: ./include/wx/wx/radiobut.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/radiobut.h
src/sourcepainter.o: ./include/wx/wx/textctrl.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/textctrl.h
src/sourcepainter.o: ./include/wx/wx/slider.h ./include/wx/wx/wx/gtk/slider.h
src/sourcepainter.o: ./include/wx/wx/gauge.h ./include/wx/wx/wx/gtk/gauge.h
src/sourcepainter.o: ./include/wx/wx/combobox.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/combobox.h ./include/wx/wx/menu.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/menu.h ./include/wx/menuitem.h
src/sourcepainter.o: ./include/wx/wx/gtk/menuitem.h ./include/wx/wx/app.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/app.h ./include/wx/wx/dialog.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/dialog.h ./include/wx/wx/timer.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/timer.h ./include/wx/wx/utils.h
src/sourcepainter.o: ./include/wx/wx/wx/filefn.h ./include/wx/wx/wx/process.h
src/sourcepainter.o: ./include/wx/wx/settings.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/settings.h
src/sourcepainter.o: ./include/wx/wx/layout.h ./include/wx/wx/mdi.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/mdi.h ./include/wx/toolbar.h
src/sourcepainter.o: ./include/wx/wx/gtk/tbargtk.h ./include/wx/wx/scrolwin.h
src/sourcepainter.o: ./include/wx/wx/wx/generic/scrolwin.h
src/sourcepainter.o: ./include/wx/wx/statusbr.h
src/sourcepainter.o: ./include/wx/wx/wx/generic/statusbr.h
src/sourcepainter.o: ./include/wx/wx/msgdlg.h
src/sourcepainter.o: ./include/wx/wx/wx/generic/msgdlgg.h
src/sourcepainter.o: ./include/wx/wx/choicdlg.h
src/sourcepainter.o: ./include/wx/wx/wx/generic/choicdgg.h
src/sourcepainter.o: ./include/wx/wx/textdlg.h
src/sourcepainter.o: ./include/wx/wx/wx/generic/textdlgg.h
src/sourcepainter.o: ./include/wx/wx/filedlg.h
src/sourcepainter.o: ./include/wx/wx/wx/gtk/filedlg.h
src/sourcepainter.o: ./include/wx/wx/dirdlg.h
src/sourcepainter.o: ./include/wx/wx/wx/generic/dirdlgg.h
src/sourcepainter.o: ./include/wx/treectrl.h
src/sourcepainter.o: ./include/wx/wx/generic/treectrl.h
src/sourcepainter.o: ./include/wx/wx/cmndata.h ./include/wx/wx/intl.h
src/sourcepainter.o: ./include/wx/wx/valtext.h ./include/sourcepainter.h
src/sourcepainter.o: ./include/markup.h
src/wx/string.o: ./include/wx/wxprec.h /usr/include/ctype.h
src/wx/string.o: /usr/include/features.h /usr/include/sys/cdefs.h
src/wx/string.o: /usr/include/gnu/stubs.h /usr/include/endian.h
src/wx/string.o: /usr/include/bytesex.h /usr/include/string.h
src/wx/string.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/wx/string.o: /usr/include/stdlib.h /usr/include/sys/types.h
src/wx/string.o: /usr/include/gnu/types.h /usr/include/time.h
src/wx/string.o: /usr/include/sys/select.h /usr/include/selectbits.h
src/wx/string.o: /usr/include/alloca.h
src/wx_extra_imps.o: ./include/wx/wxprec.h /usr/include/memory.h
src/wx_extra_imps.o: /usr/include/features.h /usr/include/sys/cdefs.h
src/wx_extra_imps.o: /usr/include/gnu/stubs.h
src/wx/object.o: ./include/wx/wxprec.h /usr/include/string.h
src/wx/object.o: /usr/include/features.h /usr/include/sys/cdefs.h
src/wx/object.o: /usr/include/gnu/stubs.h
src/wx/object.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/wx/object.o: /usr/include/assert.h
src/wx/hash.o: ./include/wx/wxprec.h ./include/wx/list.h
src/wx/hash.o: ./include/wx/wx/defs.h
src/wx/hash.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/wx/hash.o: ./include/wx/wx/setup.h ./include/wx/wx/wx/gtk/setup.h
src/wx/hash.o: ./include/wx/wx/wx/version.h ./include/wx/wx/wx/debug.h
src/wx/hash.o: /usr/include/assert.h /usr/include/features.h
src/wx/hash.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/wx/hash.o: ./include/wx/object.h ./include/wx/wx/memory.h
src/wx/hash.o: ./include/wx/wx/string.h /usr/include/string.h
src/wx/hash.o: /usr/include/stdio.h /usr/include/libio.h
src/wx/hash.o: /usr/include/_G_config.h /usr/include/gnu/types.h
src/wx/hash.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stdarg.h
src/wx/hash.o: /usr/include/stdio_lim.h
src/wx/hash.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/limits.h
src/wx/hash.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/syslimits.h
src/wx/hash.o: /usr/include/stdlib.h /usr/include/sys/types.h
src/wx/hash.o: /usr/include/time.h /usr/include/endian.h
src/wx/hash.o: /usr/include/bytesex.h /usr/include/sys/select.h
src/wx/hash.o: /usr/include/selectbits.h /usr/include/alloca.h
src/wx/hash.o: /usr/include/g++/iostream.h /usr/include/g++/streambuf.h
src/wx/hash.o: ./include/wx/wx/wx/hash.h
src/wx/log.o: ./include/wx/wxprec.h ./include/wx/file.h
src/wx/log.o: ./include/wx/textfile.h ./include/wx/wx/defs.h
src/wx/log.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/wx/log.o: ./include/wx/wx/setup.h ./include/wx/wx/wx/gtk/setup.h
src/wx/log.o: ./include/wx/wx/wx/version.h ./include/wx/wx/string.h
src/wx/log.o: /usr/include/string.h /usr/include/features.h
src/wx/log.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/wx/log.o: /usr/include/stdio.h /usr/include/libio.h
src/wx/log.o: /usr/include/_G_config.h /usr/include/gnu/types.h
src/wx/log.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stdarg.h
src/wx/log.o: /usr/include/stdio_lim.h
src/wx/log.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/limits.h
src/wx/log.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/syslimits.h
src/wx/log.o: /usr/include/stdlib.h /usr/include/sys/types.h
src/wx/log.o: /usr/include/time.h /usr/include/endian.h
src/wx/log.o: /usr/include/bytesex.h /usr/include/sys/select.h
src/wx/log.o: /usr/include/selectbits.h /usr/include/alloca.h
src/wx/log.o: ./include/wx/wx/wx/debug.h /usr/include/assert.h
src/wx/log.o: /usr/include/g++/iostream.h /usr/include/g++/streambuf.h
src/wx/log.o: ./include/wx/wx/dynarray.h ./include/wx/wx/utils.h
src/wx/log.o: ./include/wx/object.h ./include/wx/wx/memory.h
src/wx/log.o: ./include/wx/list.h ./include/wx/wx/window.h
src/wx/log.o: ./include/wx/wx/wx/gtk/window.h ./include/wx/event.h
src/wx/log.o: ./include/wx/wx/gdicmn.h ./include/wx/wx/wx/hash.h
src/wx/log.o: ./include/wx/wx/wx/colour.h ./include/wx/wx/wx/wx/gtk/colour.h
src/wx/log.o: ./include/wx/gdiobj.h ./include/wx/wx/gtk/gdiobj.h
src/wx/log.o: ./include/wx/palette.h ./include/wx/wx/gtk/palette.h
src/wx/log.o: ./include/wx/validate.h ./include/wx/cursor.h
src/wx/log.o: ./include/wx/wx/gtk/cursor.h ./include/wx/font.h
src/wx/log.o: ./include/wx/wx/gtk/font.h ./include/wx/region.h
src/wx/log.o: ./include/wx/wx/gtk/region.h ./include/wx/accel.h
src/wx/log.o: ./include/wx/wx/gtk/accel.h ./include/wx/wx/wx/filefn.h
src/wx/log.o: ./include/wx/wx/wx/process.h ./include/wx/wx/log.h
src/wx/log.o: /usr/include/errno.h /usr/include/errnos.h
src/wx/log.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
src/wx/file.o: ./include/wx/wxprec.h ./include/wx/wx/defs.h
src/wx/file.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/wx/file.o: ./include/wx/wx/setup.h ./include/wx/wx/wx/gtk/setup.h
src/wx/file.o: ./include/wx/wx/wx/version.h /usr/include/stdio.h
src/wx/file.o: /usr/include/libio.h /usr/include/features.h
src/wx/file.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/wx/file.o: /usr/include/_G_config.h /usr/include/gnu/types.h
src/wx/file.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stdarg.h
src/wx/file.o: /usr/include/stdio_lim.h /usr/include/fcntl.h
src/wx/file.o: /usr/include/fcntlbits.h /usr/include/sys/types.h
src/wx/file.o: /usr/include/time.h /usr/include/endian.h
src/wx/file.o: /usr/include/bytesex.h /usr/include/sys/select.h
src/wx/file.o: /usr/include/selectbits.h /usr/include/sys/stat.h
src/wx/file.o: /usr/include/statbuf.h ./include/wx/wx/string.h
src/wx/file.o: /usr/include/string.h
src/wx/file.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/limits.h
src/wx/file.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/syslimits.h
src/wx/file.o: /usr/include/stdlib.h /usr/include/alloca.h
src/wx/file.o: ./include/wx/wx/wx/debug.h /usr/include/assert.h
src/wx/file.o: /usr/include/g++/iostream.h /usr/include/g++/streambuf.h
src/wx/file.o: ./include/wx/wx/intl.h ./include/wx/file.h
src/wx/file.o: ./include/wx/wx/log.h
src/wx/list.o: ./include/wx/wxprec.h
src/wx/list.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stdarg.h
src/wx/list.o: /usr/include/stdlib.h /usr/include/features.h
src/wx/list.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
src/wx/list.o: /usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.60/include/stddef.h
src/wx/list.o: /usr/include/sys/types.h /usr/include/gnu/types.h
src/wx/list.o: /usr/include/time.h /usr/include/endian.h
src/wx/list.o: /usr/include/bytesex.h /usr/include/sys/select.h
src/wx/list.o: /usr/include/selectbits.h /usr/include/alloca.h
src/wx/list.o: /usr/include/string.h

# needed for unactivated
NONE =

# define library name
LIB_TARGET=wx_motif2
LIB_MAJOR=0
LIB_MINOR=1

# define library sources

LIB_CPP_SRC=\
\
 common/cmndata.cpp \
 common/config.cpp \
 common/date.cpp \
 common/docmdi.cpp \
 common/docview.cpp \
 common/dynarray.cpp \
 common/dynlib.cpp \
 common/event.cpp \
 common/file.cpp \
 common/fileconf.cpp \
 common/filefn.cpp \
 common/gdicmn.cpp \
 common/hash.cpp \
 common/helpbase.cpp \
 common/image.cpp \
 common/intl.cpp \
 common/ipcbase.cpp \
 common/layout.cpp \
 common/list.cpp \
 common/log.cpp \
 common/matrix.cpp \
 common/memory.cpp \
 common/mimetype.cpp \
 common/module.cpp \
 common/object.cpp \
 common/prntbase.cpp \
 common/resource.cpp \
 common/serbase.cpp \
 common/string.cpp \
 common/textfile.cpp \
 common/tbarbase.cpp \
 common/tbarsmpl.cpp \
 common/time.cpp \
 common/timercmn.cpp \
 common/utilscmn.cpp \
 common/wincmn.cpp \
 common/framecmn.cpp \
 common/stream.cpp \
 common/datstrm.cpp \
 common/wfstream.cpp \
 common/mstream.cpp \
 common/zstream.cpp \
 common/objstrm.cpp \
 common/sckstrm.cpp \
 common/validate.cpp \
 common/valtext.cpp \
 common/variant.cpp \
 common/wxexpr.cpp \
 common/socket.cpp \
 common/sckaddr.cpp \
 common/sckipc.cpp \
 common/protocol.cpp \
 common/ftp.cpp \
 common/http.cpp \
 common/url.cpp \
 common/tokenzr.cpp \
\
 motif/accel.cpp \
 motif/app.cpp \
 motif/bitmap.cpp \
 motif/bmpbuttn.cpp \
 motif/brush.cpp \
 motif/button.cpp \
 motif/checkbox.cpp \
 motif/choice.cpp \
 motif/clipbrd.cpp \
 motif/colour.cpp \
 motif/colordlg.cpp \
 motif/control.cpp \
 motif/combobox.cpp \
 motif/cursor.cpp \
 motif/data.cpp \
 motif/dc.cpp \
 motif/dcclient.cpp \
 motif/dcmemory.cpp \
 motif/dcscreen.cpp \
 motif/dialog.cpp \
 motif/dirdlg.cpp \
 motif/dnd.cpp \
 motif/filedlg.cpp \
 motif/font.cpp \
 motif/fontdlg.cpp \
 motif/frame.cpp \
 motif/gauge.cpp \
 motif/gdiobj.cpp \
 motif/helpxxxx.cpp \
 motif/icon.cpp \
 motif/listbox.cpp \
 motif/joystick.cpp \
 motif/main.cpp \
 motif/mdi.cpp \
 motif/menu.cpp \
 motif/menuitem.cpp \
 motif/metafile.cpp \
 motif/minifram.cpp \
 motif/msgdlg.cpp \
 motif/palette.cpp \
 motif/pen.cpp \
 motif/print.cpp \
 motif/radiobox.cpp \
 motif/radiobut.cpp \
 motif/region.cpp \
 motif/scrolbar.cpp \
 motif/settings.cpp \
 motif/slider.cpp \
 motif/spinbutt.cpp \
 motif/statbox.cpp \
 motif/statbmp.cpp \
 motif/stattext.cpp \
 motif/taskbar.cpp \
 motif/textctrl.cpp \
 motif/thread.cpp \
 motif/timer.cpp \
 motif/toolbar.cpp \
 motif/utils.cpp \
 motif/utilsexc.cpp \
 motif/wave.cpp \
 motif/window.cpp \
\
 generic/choicdgg.cpp \
 generic/colrdlgg.cpp \
 generic/dcpsg.cpp \
 generic/fontdlgg.cpp \
 generic/gridg.cpp \
 generic/helpext.cpp \
 generic/helphtml.cpp \
 generic/imaglist.cpp \
 generic/listctrl.cpp \
 generic/laywin.cpp \
 generic/msgdlgg.cpp \
 generic/notebook.cpp \
 generic/panelg.cpp \
 generic/printps.cpp \
 generic/prntdlgg.cpp \
 generic/prop.cpp \
 generic/propform.cpp \
 generic/proplist.cpp \
 generic/sashwin.cpp \
 generic/scrolwin.cpp \
 generic/splitter.cpp \
 generic/statusbr.cpp \
 generic/tabg.cpp \
 generic/textdlgg.cpp \
 generic/treectrl.cpp
  
LIB_C_SRC=\
\
 motif/xmcombo/xmcombo.c \
\
 common/extended.c

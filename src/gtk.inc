# needed for unactivated
NONE =

# define library name
LIB_TARGET=wx_gtk2
LIB_MAJOR=1
LIB_MINOR=0

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
 common/intl.cpp \
 common/image.cpp \
 common/imagjpeg.cpp \
 common/imagpng.cpp \
 common/imaggif.cpp \
 common/ipcbase.cpp \
 common/layout.cpp \
 common/list.cpp \
 common/log.cpp \
 common/matrix.cpp \
 common/memory.cpp \
 common/mimetype.cpp \
 common/module.cpp \
 common/object.cpp \
 common/db.cpp \
 common/dbtable.cpp \
 common/prntbase.cpp \
 common/serbase.cpp \
 common/paper.cpp \
 common/string.cpp \
 common/textfile.cpp \
 common/time.cpp \
 common/timercmn.cpp \
 common/tbarsmpl.cpp \
 common/utilscmn.cpp \
 common/wincmn.cpp \
 common/framecmn.cpp \
 common/stream.cpp \
 common/wfstream.cpp \
 common/datstrm.cpp \
 common/mstream.cpp \
 common/zstream.cpp \
 common/objstrm.cpp \
 common/sckstrm.cpp \
 common/validate.cpp \
 common/valgen.cpp \
 common/valtext.cpp \
 common/variant.cpp \
 common/socket.cpp \
 common/sckaddr.cpp \
 common/sckipc.cpp \
 common/protocol.cpp \
 common/http.cpp \
 common/ftp.cpp \
 common/url.cpp \
 common/tbarbase.cpp \
 common/tokenzr.cpp \
 common/resource.cpp \
 common/wxexpr.cpp \
 common/process.cpp \
\
 gtk/accel.cpp \
 gtk/app.cpp \
 gtk/bitmap.cpp \
 gtk/bmpbuttn.cpp \
 gtk/brush.cpp \
 gtk/button.cpp \
 gtk/checkbox.cpp \
 gtk/checklst.cpp \
 gtk/choice.cpp \
 gtk/clipbrd.cpp \
 gtk/colour.cpp \
 gtk/control.cpp \
 gtk/combobox.cpp \
 gtk/cursor.cpp \
 gtk/data.cpp \
 gtk/dataobj.cpp \
 gtk/dc.cpp \
 gtk/dcclient.cpp \
 gtk/dcmemory.cpp \
 gtk/dcscreen.cpp \
 gtk/dnd.cpp \
 gtk/dialog.cpp \
 gtk/filedlg.cpp \
 gtk/font.cpp \
 gtk/frame.cpp \
 gtk/gauge.cpp \
 gtk/gdiobj.cpp \
 gtk/icon.cpp \
 gtk/listbox.cpp \
 gtk/main.cpp \
 gtk/mdi.cpp \
 gtk/menu.cpp \
 gtk/minifram.cpp \
 gtk/notebook.cpp \
 gtk/palette.cpp \
 gtk/pen.cpp \
 gtk/radiobox.cpp \
 gtk/radiobut.cpp \
 gtk/region.cpp \
 gtk/scrolbar.cpp \
 gtk/settings.cpp \
 gtk/slider.cpp \
 gtk/spinbutt.cpp \
 gtk/statbox.cpp \
 gtk/statbmp.cpp \
 gtk/stattext.cpp \
 gtk/tbargtk.cpp \
 gtk/textctrl.cpp \
 gtk/timer.cpp \
 gtk/tooltip.cpp \
 gtk/utilsgtk.cpp \
 gtk/utilsres.cpp \
 gtk/window.cpp \
\
 unix/utilsunx.cpp \
\
 generic/choicdgg.cpp \
 generic/dcpsg.cpp \
 generic/colrdlgg.cpp \
 generic/dirdlgg.cpp \
 generic/fontdlgg.cpp \
 generic/gridg.cpp \
 generic/imaglist.cpp \
 generic/listctrl.cpp \
 generic/laywin.cpp \
 generic/msgdlgg.cpp \
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
 generic/treectrl.cpp \
 generic/helpext.cpp \
 generic/helphtml.cpp

LIB_C_SRC=\
 common/extended.c \
\
 gtk/win_gtk.c

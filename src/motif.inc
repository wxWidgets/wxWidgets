# needed for unactivated
NONE =

# define library name
LIB_TARGET=wx_motif
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
 common/intl.cpp \
 common/image.cpp \
 common/ipcbase.cpp \
 common/layout.cpp \
 common/list.cpp \
 common/log.cpp \
 common/matrix.cpp \
 common/memory.cpp \
 common/module.cpp \
 common/object.cpp \
 common/postscrp.cpp \
 common/prntbase.cpp \
 common/resource.cpp \
 common/serbase.cpp \
 common/string.cpp \
 common/tbarbase.cpp \
 common/textfile.cpp \
 common/time.cpp \
 common/timercmn.cpp \
 common/utilscmn.cpp \
 common/wincmn.cpp \
 common/framecmn.cpp \
 common/stream.cpp \
 common/datstrm.cpp \
 common/fstream.cpp \
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
 motif/colordlg.cpp \
 motif/colour.cpp \
 motif/control.cpp \
 motif/combobox.cpp \
 motif/cursor.cpp \
 motif/data.cpp \
 motif/dc.cpp \
 motif/dcclient.cpp \
 motif/dcmemory.cpp \
 motif/dcscreen.cpp \
 motif/dialog.cpp \
 motif/filedlg.cpp \
 motif/font.cpp \
 motif/fontdlg.cpp \
 motif/frame.cpp \
 motif/gauge.cpp \
 motif/gdiobj.cpp \
 motif/icon.cpp \
 motif/listbox.cpp \
 motif/main.cpp \
 motif/mdi.cpp \
 motif/menu.cpp \
 motif/menuitem.cpp \
 motif/msgdlg.cpp \
 motif/notebook.cpp \
 motif/palette.cpp \
 motif/pen.cpp \
 motif/print.cpp \
 motif/printdlg.cpp \
 motif/radiobox.cpp \
 motif/radiobut.cpp \
 motif/region.cpp \
 motif/scrolbar.cpp \
 motif/settings.cpp \
 motif/spinbutt.cpp \
 motif/slider.cpp \
 motif/statbox.cpp \
 motif/statbmp.cpp \
 motif/stattext.cpp \
 motif/toolbar.cpp \
 motif/textctrl.cpp \
 motif/timer.cpp \
 motif/utils.cpp \
 motif/utilsexc.cpp \
 motif/window.cpp \
\
 generic/choicdgg.cpp \
 generic/colrdlgg.cpp \
 generic/fontdlgg.cpp \
 generic/gridg.cpp \
 generic/helpext.cpp \
 generic/imaglist.cpp \
 generic/listctrl.cpp \
 generic/laywin.cpp \
 generic/msgdlgg.cpp \
 generic/panelg.cpp \
 generic/printps.cpp \
 generic/prntdlgg.cpp \
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
 common/extended.c \
\
 png/png.c \
 png/pngset.c \
 png/pngget.c \
 png/pngrutil.c \
 png/pngtrans.c \
 png/pngwutil.c \
 png/pngread.c \
 png/pngrio.c \
 png/pngwio.c \
 png/pngwrite.c \
 png/pngrtran.c \
 png/pngwtran.c \
 png/pngmem.c \
 png/pngerror.c \
 png/pngpread.c \
\
 zlib/adler32.c \
 zlib/compress.c \
 zlib/crc32.c \
 zlib/gzio.c \
 zlib/uncompr.c \
 zlib/deflate.c \
 zlib/trees.c \
 zlib/zutil.c \
 zlib/inflate.c \
 zlib/infblock.c \
 zlib/inftrees.c \
 zlib/infcodes.c \
 zlib/infutil.c \
 zlib/inffast.c \

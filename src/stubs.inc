# needed for unactivated
NONE =

# define library name
LIB_TARGET=wx_stubs
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
 common/ipcbase.cpp \
 common/layout.cpp \
 common/list.cpp \
 common/log.cpp \
 common/matrix.cpp \
 common/memory.cpp \
 common/module.cpp \
 common/object.cpp \
 common/odbc.cpp \
 common/prntbase.cpp \
 common/resource.cpp \
 common/serbase.cpp \
 common/string.cpp \
 common/textfile.cpp \
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
 stubs/accel.cpp \
 stubs/app.cpp \
 stubs/bitmap.cpp \
 stubs/bmpbuttn.cpp \
 stubs/brush.cpp \
 stubs/button.cpp \
 stubs/checkbox.cpp \
 stubs/choice.cpp \
 stubs/clipbrd.cpp \
 stubs/colour.cpp \
 stubs/colordlg.cpp \
 stubs/control.cpp \
 stubs/combobox.cpp \
 stubs/cursor.cpp \
 stubs/data.cpp \
 stubs/dc.cpp \
 stubs/dcclient.cpp \
 stubs/dcmemory.cpp \
 stubs/dcscreen.cpp \
 stubs/dialog.cpp \
 stubs/dirdlg.cpp \
 stubs/dnd.cpp \
 stubs/filedlg.cpp \
 stubs/font.cpp \
 stubs/fontdlg.cpp \
 stubs/frame.cpp \
 stubs/gauge.cpp \
 stubs/gdiobj.cpp \
 stubs/helpxxxx.cpp \
 stubs/icon.cpp \
 stubs/imaglist.cpp \
 stubs/listbox.cpp \
 stubs/joystick.cpp \
 stubs/main.cpp \
 stubs/mdi.cpp \
 stubs/menu.cpp \
 stubs/menuitem.cpp \
 stubs/metafile.cpp \
 stubs/minifram.cpp \
 stubs/msgdlg.cpp \
 stubs/notebook.cpp \
 stubs/palette.cpp \
 stubs/pen.cpp \
 stubs/print.cpp \
 stubs/radiobox.cpp \
 stubs/radiobut.cpp \
 stubs/region.cpp \
 stubs/scrolbar.cpp \
 stubs/settings.cpp \
 stubs/slider.cpp \
 stubs/spinbutt.cpp \
 stubs/statbox.cpp \
 stubs/statbmp.cpp \
 stubs/stattext.cpp \
 stubs/statusbr.cpp \
 stubs/taskbar.cpp \
 stubs/textctrl.cpp \
 stubs/thread.cpp \
 stubs/timer.cpp \
 stubs/toolbar.cpp \
 stubs/treectrl.cpp \
 stubs/utils.cpp \
 stubs/utilsexc.cpp \
 stubs/wave.cpp \
 stubs/window.cpp \
\
 generic/choicdgg.cpp \
 generic/colrdlgg.cpp \
 generic/dcpsg.cpp \
 generic/fontdlgg.cpp \
 generic/gridg.cpp \
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
 common/extended.c \
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
\
  iodbc/dlf.c \
  iodbc/dlproc.c \
  iodbc/herr.c \
  iodbc/henv.c \
  iodbc/hdbc.c \
  iodbc/hstmt.c \
  iodbc/connect.c \
  iodbc/prepare.c \
  iodbc/result.c \
  iodbc/execute.c \
  iodbc/fetch.c \
  iodbc/info.c \
  iodbc/catalog.c \
  iodbc/misc.c \
  iodbc/itrace.c

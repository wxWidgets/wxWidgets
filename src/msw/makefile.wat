#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1998
#
# Makefile : Builds wxWindows library for Watcom C++, WIN32

WXDIR = ..\..

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib

LIBTARGET   = $(WXLIB)\wx.lib
DUMMY=dummydll
# ODBCLIB     = ..\..\contrib\odbc\odbc32.lib

EXTRATARGETS = xpm png zlib
EXTRATARGETSCLEAN = clean_xpm clean_png clean_zlib
GENDIR=$(WXDIR)\src\generic
COMMDIR=$(WXDIR)\src\common
XPMDIR=$(WXDIR)\src\xpm
OLEDIR=ole
MSWDIR=$(WXDIR)\src\msw

DOCDIR = $(WXDIR)\docs

GENERICOBJS= choicdgg.obj &
  gridg.obj &
  laywin.obj &
  panelg.obj &
  prop.obj &
  propform.obj &
  proplist.obj &
  sashwin.obj &
  scrolwin.obj &
  splitter.obj &
  statusbr.obj &
  tabg.obj &
  textdlgg.obj

# These are generic things that don't need to be compiled on MSW,
# but sometimes it's useful to do so for testing purposes.
NONESSENTIALOBJS= printps.obj &
  prntdlgg.obj &
  msgdlgg.obj &
  helpxlp.obj &
  colrdlgg.obj &
  fontdlgg.obj &
  postscrp.obj

COMMONOBJS = cmndata.obj &
  config.obj &
  docview.obj &
  docmdi.obj &
  dynarray.obj &
  dynlib.obj &
  event.obj &
  file.obj &
  filefn.obj &
  fileconf.obj &
  framecmn.obj &
  gdicmn.obj &
  image.obj &
  intl.obj &
  ipcbase.obj &
  helpbase.obj &
  layout.obj &
  log.obj &
  memory.obj &
  mimetype.obj &
  module.obj &
  object.obj &
  prntbase.obj &
  resource.obj &
  tbarbase.obj &
  tbarsmpl.obj &
  textfile.obj &
  timercmn.obj &
  utilscmn.obj &
  validate.obj &
  valtext.obj &
  date.obj &
  hash.obj &
  list.obj &
  string.obj &
  socket.obj &
  sckaddr.obj &
  sckfile.obj &
  sckipc.obj &
  sckstrm.obj &
  url.obj &
  http.obj &
  protocol.obj &
  time.obj &
  tokenzr.obj &
  wxexpr.obj &
  y_tab.obj &
  extended.obj &
  process.obj &
  wfstream.obj &
  mstream.obj &
  zstream.obj &
  stream.obj &
  datstrm.obj &
  objstrm.obj &
  variant.obj &
  wincmn.obj

# Can't compile these yet under Watcom C++
#  odbc.obj &
#  db.obj &
#  dbtable.obj &

MSWOBJS = &
  accel.obj &
  app.obj &
  bitmap.obj &
  bmpbuttn.obj &
  brush.obj &
  button.obj &
  checkbox.obj &
  checklst.obj &
  choice.obj &
  clipbrd.obj &
  colordlg.obj &
  colour.obj &
  combobox.obj &
  control.obj &
  curico.obj &
  cursor.obj &
  data.obj &
  dc.obj &
  dcmemory.obj &
  dcclient.obj &
  dcprint.obj &
  dcscreen.obj &
  dde.obj &
  dialog.obj &
  dib.obj &
  dibutils.obj &
  dirdlg.obj &
  filedlg.obj &
  font.obj &
  fontdlg.obj &
  frame.obj &
  gauge95.obj &
  gaugemsw.obj &
  gdiobj.obj &
  helpwin.obj &
  icon.obj &
  imaglist.obj &
  iniconf.obj &
  joystick.obj &
  listbox.obj &
  listctrl.obj &
  main.obj &
  mdi.obj &
  menu.obj &
  menuitem.obj &
  metafile.obj &
  minifram.obj &
  msgdlg.obj &
  nativdlg.obj &
  notebook.obj &
  ownerdrw.obj &
  palette.obj &
  pen.obj &
  penwin.obj &
  printdlg.obj &
  printwin.obj &
  radiobox.obj &
  radiobut.obj &
  region.obj &
  registry.obj &
  regconf.obj &
  scrolbar.obj &
  settings.obj &
  slidrmsw.obj &
  slider95.obj &
  spinbutt.obj &
  statbmp.obj &
  statbox.obj &
  statbr95.obj &
  stattext.obj &
  tabctrl.obj &
  taskbar.obj &
  tbar95.obj &
  tbarmsw.obj &
  textctrl.obj &
  thread.obj &
  timer.obj &
  treectrl.obj &
  utils.obj &
  utilsexc.obj &
  wave.obj &
  window.obj &

#  pnghand.obj &

OLEOBJS = &
  droptgt.obj &
  dropsrc.obj &
  dataobj.obj &
  oleutils.obj &
  uuid.obj &
  automtn.obj

# Add $(NONESSENTIALOBJS) if wanting generic dialogs, PostScript etc.
OBJECTS = $(COMMONOBJS) $(GENERICOBJS) $(MSWOBJS) $(OLEOBJS)

all:        $(OBJECTS) $(LIBTARGET) $(EXTRATARGETS)

$(LIBTARGET) : $(OBJECTS)
    %create tmp.lbc
    @for %i in ( $(OBJECTS) ) do @%append tmp.lbc +%i
    wlib /b /c /n /p=512 $^@ @tmp.lbc

#test : $(OBJECTS)
#    %create tmp.lbc
#    @for %i in ( $(OBJECTS) ) do @%append tmp.lbc +%i
#    wlib /b /c /n /p=512 $^@ @tmp.lbc
	
	
clean:   $(EXTRATARGETSCLEAN)
    -erase *.obj
    -erase $(LIBTARGET)
    -erase *.pch
    -erase *.err
    -erase *.lbc

cleanall:   clean

accel.obj:     $(MSWDIR)\accel.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

app.obj:     $(MSWDIR)\app.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

bitmap.obj:     $(MSWDIR)\bitmap.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

bmpbuttn.obj:     $(MSWDIR)\bmpbuttn.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

brush.obj:     $(MSWDIR)\brush.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

button.obj:     $(MSWDIR)\button.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

choice.obj:     $(MSWDIR)\choice.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

checkbox.obj:     $(MSWDIR)\checkbox.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

checklst.obj:     $(MSWDIR)\checklst.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

clipbrd.obj:     $(MSWDIR)\clipbrd.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

colordlg.obj:     $(MSWDIR)\colordlg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

colour.obj:     $(MSWDIR)\colour.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

combobox.obj:     $(MSWDIR)\combobox.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

control.obj:     $(MSWDIR)\control.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

curico.obj:     $(MSWDIR)\curico.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

cursor.obj:     $(MSWDIR)\cursor.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

data.obj:     $(MSWDIR)\data.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dde.obj:     $(MSWDIR)\dde.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dc.obj:     $(MSWDIR)\dc.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dcmemory.obj:     $(MSWDIR)\dcmemory.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dcclient.obj:     $(MSWDIR)\dcclient.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dcprint.obj:     $(MSWDIR)\dcprint.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dcscreen.obj:     $(MSWDIR)\dcscreen.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dialog.obj:     $(MSWDIR)\dialog.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dib.obj:     $(MSWDIR)\dib.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dibutils.obj:     $(MSWDIR)\dibutils.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dirdlg.obj:     $(MSWDIR)\dirdlg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

filedlg.obj:     $(MSWDIR)\filedlg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

font.obj:     $(MSWDIR)\font.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

fontdlg.obj:     $(MSWDIR)\fontdlg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

frame.obj:     $(MSWDIR)\frame.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

gauge95.obj:     $(MSWDIR)\gauge95.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

gaugemsw.obj:     $(MSWDIR)\gaugemsw.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

gdiobj.obj:     $(MSWDIR)\gdiobj.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

icon.obj:     $(MSWDIR)\icon.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

imaglist.obj:     $(MSWDIR)\imaglist.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

joystick.obj:     $(MSWDIR)\joystick.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

listbox.obj:     $(MSWDIR)\listbox.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

listctrl.obj:     $(MSWDIR)\listctrl.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

main.obj:     $(MSWDIR)\main.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

mdi.obj:     $(MSWDIR)\mdi.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

menu.obj:     $(MSWDIR)\menu.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

menuitem.obj:     $(MSWDIR)\menuitem.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

metafile.obj:     $(MSWDIR)\metafile.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

minifram.obj:     $(MSWDIR)\minifram.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

msgdlg.obj:     $(MSWDIR)\msgdlg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

nativdlg.obj:     $(MSWDIR)\nativdlg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

notebook.obj:     $(MSWDIR)\notebook.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

ownerdrw.obj:     $(MSWDIR)\ownerdrw.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

palette.obj:     $(MSWDIR)\palette.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

pen.obj:     $(MSWDIR)\pen.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

penwin.obj:     $(MSWDIR)\penwin.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

pnghand.obj:     $(MSWDIR)\pnghand.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

printdlg.obj:     $(MSWDIR)\printdlg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

printwin.obj:     $(MSWDIR)\printwin.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

radiobox.obj:     $(MSWDIR)\radiobox.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

radiobut.obj:     $(MSWDIR)\radiobut.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

region.obj:     $(MSWDIR)\region.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

registry.obj:     $(MSWDIR)\registry.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

regconf.obj:     $(MSWDIR)\regconf.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

scrolbar.obj:     $(MSWDIR)\scrolbar.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

settings.obj:     $(MSWDIR)\settings.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

slidrmsw.obj:     $(MSWDIR)\slidrmsw.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

slider95.obj:     $(MSWDIR)\slider95.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

spinbutt.obj:     $(MSWDIR)\spinbutt.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

statbmp.obj:     $(MSWDIR)\statbmp.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

statbox.obj:     $(MSWDIR)\statbox.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

statbr95.obj:     $(MSWDIR)\statbr95.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

stattext.obj:     $(MSWDIR)\stattext.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

tabctrl.obj:     $(MSWDIR)\tabctrl.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

taskbar.obj:     $(MSWDIR)\taskbar.cpp
        cl @<<
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

tbar95.obj:     $(MSWDIR)\tbar95.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

tbarmsw.obj:     $(MSWDIR)\tbarmsw.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

textctrl.obj:     $(MSWDIR)\textctrl.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

thread.obj:     $(MSWDIR)\thread.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

timer.obj:     $(MSWDIR)\timer.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

treectrl.obj:     $(MSWDIR)\treectrl.cpp
        cl @<<
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

utils.obj:     $(MSWDIR)\utils.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

utilsexc.obj:     $(MSWDIR)\utilsexc.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

wave.obj:     $(MSWDIR)\wave.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

window.obj:     $(MSWDIR)\window.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

droptgt.obj:     $(OLEDIR)\droptgt.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dropsrc.obj:     $(OLEDIR)\dropsrc.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dataobj.obj:     $(OLEDIR)\dataobj.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

oleutils.obj:     $(OLEDIR)\oleutils.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

uuid.obj:     $(OLEDIR)\uuid.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

automtn.obj:     $(OLEDIR)\automtn.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

########################################################
# Common objects (always compiled)

cmndata.obj:     $(COMMDIR)\cmndata.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

config.obj:     $(COMMDIR)\config.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

db.obj:     $(COMMDIR)\db.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dbtable.obj:     $(COMMDIR)\dbtable.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

docview.obj:     $(COMMDIR)\docview.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

docmdi.obj:     $(COMMDIR)\docmdi.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dynarray.obj:     $(COMMDIR)\dynarray.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

dynlib.obj:     $(COMMDIR)\dynlib.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

event.obj:     $(COMMDIR)\event.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

file.obj:     $(COMMDIR)\file.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

fileconf.obj:     $(COMMDIR)\fileconf.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

filefn.obj:     $(COMMDIR)\filefn.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

framecmn.obj:     $(COMMDIR)\framecmn.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

gdicmn.obj:     $(COMMDIR)\gdicmn.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

image.obj:     $(COMMDIR)\image.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

intl.obj:     $(COMMDIR)\intl.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

ipcbase.obj:     $(COMMDIR)\ipcbase.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

helpbase.obj:     $(COMMDIR)\helpbase.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

layout.obj:     $(COMMDIR)\layout.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

log.obj:     $(COMMDIR)\log.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

memory.obj:     $(COMMDIR)\memory.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

mimetype.obj:     $(COMMDIR)\mimetype.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

module.obj:     $(COMMDIR)\module.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

object.obj:     $(COMMDIR)\object.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

odbc.obj:     $(COMMDIR)\odbc.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

postscrp.obj:     $(COMMDIR)\postcrp.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

prntbase.obj:     $(COMMDIR)\prntbase.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

resource.obj:     $(COMMDIR)\resource.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

tbarbase.obj:     $(COMMDIR)\tbarbase.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

tbarsmpl.obj:     $(COMMDIR)\tbarsmpl.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

textfile.obj:     $(COMMDIR)\textfile.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

timercmn.obj:     $(COMMDIR)\timercmn.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

utilscmn.obj:     $(COMMDIR)\utilscmn.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

validate.obj:     $(COMMDIR)\validate.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

valtext.obj:     $(COMMDIR)\valtext.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

date.obj:     $(COMMDIR)\date.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

wxexpr.obj:     $(COMMDIR)\wxexpr.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

hash.obj:     $(COMMDIR)\hash.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

list.obj:     $(COMMDIR)\list.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

string.obj:     $(COMMDIR)\string.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

socket.obj:     $(COMMDIR)\socket.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

sckaddr.obj:     $(COMMDIR)\sckaddr.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

sckfile.obj:     $(COMMDIR)\sckfile.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

sckipc.obj:     $(COMMDIR)\sckipc.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

sckstrm.obj:     $(COMMDIR)\sckstrm.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

url.obj:     $(COMMDIR)\url.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

http.obj:     $(COMMDIR)\http.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

protocol.obj:     $(COMMDIR)\protocol.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

tokenzr.obj:     $(COMMDIR)\tokenzr.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

matrix.obj:     $(COMMDIR)\matrix.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

time.obj:     $(COMMDIR)\time.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

stream.obj:     $(COMMDIR)\stream.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

wfstream.obj:     $(COMMDIR)\wfstream.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

mstream.obj:     $(COMMDIR)\mstream.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

zstream.obj:     $(COMMDIR)\zstream.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

datstrm.obj:     $(COMMDIR)\datstrm.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

objstrm.obj:     $(COMMDIR)\objstrm.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

extended.obj:     $(COMMDIR)\extended.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $(COMMDIR)\extended.c

process.obj:     $(COMMDIR)\process.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

variant.obj:     $(COMMDIR)\variant.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

wincmn.obj:     $(COMMDIR)\wincmn.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

y_tab.obj:     $(COMMDIR)\y_tab.c $(COMMDIR)\lex_yy.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) -DUSE_DEFINE $(COMMDIR)\y_tab.c

#  *$(CC) $(CPPFLAGS) $(IFLAGS) -DUSE_DEFINE -DYY_USE_PROTOS $(COMMDIR)\y_tab.c

$(COMMDIR)\y_tab.c:     $(COMMDIR)\dosyacc.c
        copy $(COMMDIR)\dosyacc.c $(COMMDIR)\y_tab.c

$(COMMDIR)\lex_yy.c:    $(COMMDIR)\doslex.c
    copy $(COMMDIR)\doslex.c $(COMMDIR)\lex_yy.c

########################################################
# Generic objects (not always compiled, depending on
# whether platforms have native implementations)

choicdgg.obj:     $(GENDIR)\choicdgg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

colrdlgg.obj:     $(GENDIR)\colrdgg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

fontdlgg.obj:     $(GENDIR)\fontdlgg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

gridg.obj:     $(GENDIR)\gridg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

helpxlp.obj:     $(GENDIR)\helpxlp.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

laywin.obj:     $(GENDIR)\laywin.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

msgdlgg.obj:     $(GENDIR)\msgdlgg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

panelg.obj:     $(GENDIR)\panelg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

printps.obj:     $(GENDIR)\printps.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

prop.obj:     $(GENDIR)\prop.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

propform.obj:     $(GENDIR)\propform.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

proplist.obj:     $(GENDIR)\proplist.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

prntdlgg.obj:     $(GENDIR)\prntdlgg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

sashwin.obj:     $(GENDIR)\sashwin.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

scrolwin.obj:     $(GENDIR)\scrolwin.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

splitter.obj:     $(GENDIR)\splitter.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

statusbr.obj:     $(GENDIR)\statusbr.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

tabg.obj:     $(GENDIR)\tabg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

textdlgg.obj: $(GENDIR)\textdlgg.cpp
  *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

crbuffri.obj: $(XPMDIR)\crbuffri.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

crbuffrp.obj: $(XPMDIR)\crbuffrp.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

crdatfri.obj: $(XPMDIR)\crdatfri.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

crdatfrp.obj: $(XPMDIR)\crdatfrp.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

create.obj: $(XPMDIR)\create.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

crifrbuf.obj: $(XPMDIR)\crifrbuf.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

crifrdat.obj: $(XPMDIR)\crifrdat.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

crpfrbuf.obj: $(XPMDIR)\crpfrbuf.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

crpfrdat.obj: $(XPMDIR)\crpfrdat.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

# TODO: what to do about this clash of filename????
#data.obj: $(XPMDIR)\data.c
#  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

hashtab.obj: $(XPMDIR)\hashtab.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

misc.obj: $(XPMDIR)\misc.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

parse.obj: $(XPMDIR)\parse.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

rdftodat.obj: $(XPMDIR)\rdftodat.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

rdftoi.obj: $(XPMDIR)\rdftoi.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

rdftop.obj: $(XPMDIR)\rdftop.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

rgb.obj: $(XPMDIR)\rgb.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

scan.obj: $(XPMDIR)\scan.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

simx.obj: $(XPMDIR)\simx.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

wrffrdat.obj: $(XPMDIR)\wrffrdat.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

wrffri.obj: $(XPMDIR)\wrffri.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

wrffrp.obj: $(XPMDIR)\wrffrp.c
  *$(CC) $(CPPFLAGS) $(IFLAGS) $<

OBJ1 = adler32$(O) compress$(O) crc32$(O) gzio$(O) uncompr$(O) deflate$(O) \
  trees$(O) 
OBJ2 = zutil$(O) inflate$(O) infblock$(O) inftrees$(O) infcodes$(O) \
  infutil$(O) inffast$(O) 

adler32.obj: adler32.c zutil.h zlib.h zconf.h
	$(CC) -c $(CFLAGS) $*.c

compress.obj: compress.c zlib.h zconf.h
	$(CC) -c $(CFLAGS) $*.c

crc32.obj: crc32.c zutil.h zlib.h zconf.h
	$(CC) -c $(CFLAGS) $*.c

deflate.obj: deflate.c deflate.h zutil.h zlib.h zconf.h
	$(CC) -c $(CFLAGS) $*.c

gzio.obj: gzio.c zutil.h zlib.h zconf.h
	$(CC) -c $(CFLAGS) $*.c

infblock.obj: infblock.c zutil.h zlib.h zconf.h infblock.h inftrees.h\
   infcodes.h infutil.h
	$(CC) -c $(CFLAGS) $*.c

infcodes.obj: infcodes.c zutil.h zlib.h zconf.h inftrees.h infutil.h\
   infcodes.h inffast.h
	$(CC) -c $(CFLAGS) $*.c

inflate.obj: inflate.c zutil.h zlib.h zconf.h infblock.h
	$(CC) -c $(CFLAGS) $*.c

inftrees.obj: inftrees.c zutil.h zlib.h zconf.h inftrees.h
	$(CC) -c $(CFLAGS) $*.c

infutil.obj: infutil.c zutil.h zlib.h zconf.h inftrees.h infutil.h
	$(CC) -c $(CFLAGS) $*.c

inffast.obj: inffast.c zutil.h zlib.h zconf.h inftrees.h infutil.h inffast.h
	$(CC) -c $(CFLAGS) $*.c

trees.obj: trees.c deflate.h zutil.h zlib.h zconf.h
	$(CC) -c $(CFLAGS) $*.c

uncompr.obj: uncompr.c zlib.h zconf.h
	$(CC) -c $(CFLAGS) $*.c

zutil.obj: zutil.c zutil.h zlib.h zconf.h
	$(CC) -c $(CFLAGS) $*.c

xpm:   .SYMBOLIC
    cd $(WXDIR)\src\xpm
    wmake -f makefile.wat all
    cd $(WXDIR)\src\msw

clean_xpm:   .SYMBOLIC
    cd $(WXDIR)\src\xpm
    wmake -f makefile.wat clean
    cd $(WXDIR)\src\msw

png:   .SYMBOLIC
    cd $(WXDIR)\src\png
    wmake -f makefile.wat all
    cd $(WXDIR)\src\msw

clean_png:   .SYMBOLIC
    cd $(WXDIR)\src\png
    wmake -f makefile.wat clean
    cd $(WXDIR)\src\msw

zlib:   .SYMBOLIC
    cd $(WXDIR)\src\zlib
    wmake -f makefile.wat all
    cd $(WXDIR)\src\msw

clean_zlib:   .SYMBOLIC
    cd $(WXDIR)\src\zlib
    wmake -f makefile.wat clean
    cd $(WXDIR)\src\msw



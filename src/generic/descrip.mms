#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 9 November 1999                                                     *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm
.else
CXX_DEFINE =
.endif
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp

OBJECTS = \
		busyinfo.obj,\
		calctrl.obj,\
		caret.obj,\
		choicdgg.obj,\
		colrdlgg.obj,\
		dcpsg.obj,\
		dirctrlg.obj,\
		dirdlgg.obj,\
		fontdlgg.obj,\
		grid.obj,\
		helpext.obj,\
		helphtml.obj,\
		helpwxht.obj,\
		helpxlp.obj,\
		imaglist.obj,\
		laywin.obj,\
		listctrl.obj,\
		logg.obj,\
		msgdlgg.obj,\
		numdlgg.obj,\
		panelg.obj,\
		plot.obj,\
		printps.obj,\
		prntdlgg.obj,\
		progdlgg.obj,\
		prop.obj,\
		propform.obj,\
		proplist.obj,\
		sashwin.obj,\
		scrolwin.obj,\
		splitter.obj,\
		statusbr.obj,\
		tbarsmpl.obj,\
		tabg.obj,\
		textdlgg.obj,\
		tipdlg.obj,\
		treectlg.obj,\
		wizard.obj

SOURCES = \
		busyinfo.cpp,\
		calctrl.cpp,\
		caret.cpp,\
		choicdgg.cpp,\
		colrdlgg.cpp,\
		dcpsg.cpp,\
		dirctrlg.cpp,\
		dirdlgg.cpp,\
		filedlgg.cpp,\
		fontdlgg.cpp,\
		grid.cpp,\
		helpext.cpp,\
		helphtml.cpp,\
		helpwxht.cpp,\
		helpxlp.cpp,\
		imaglist.cpp,\
		laywin.cpp,\
		listctrl.cpp,\
		logg.cpp,\
		msgdlgg.cpp,\
		notebook.cpp,\
		numdlgg.cpp,\
		panelg.cpp,\
		plot.cpp,\
		printps.cpp,\
		prntdlgg.cpp,\
		progdlgg.cpp,\
		prop.cpp,\
		propform.cpp,\
		proplist.cpp,\
		sashwin.cpp,\
		scrolwin.cpp,\
		splitter.cpp,\
		statline.cpp,\
		statusbr.cpp,\
		tbarsmpl.cpp,\
		tabg.cpp,\
		textdlgg.cpp,\
		tipdlg.cpp,\
		treectlg.cpp,\
		wizard.cpp

.ifdef __WXMOTIF__
OBJECTS0=,statline.obj,\
		notebook.obj
.else
OBJECTS0=,filedlgg.obj

.endif

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)$(OBJECTS0)
.ifdef __WXMOTIF__
	library/crea [--.lib]libwx_motif.olb $(OBJECTS)$(OBJECTS0)
.else
.ifdef __WXGTK__
	library/crea [--.lib]libwx_gtk.olb $(OBJECTS)$(OBJECTS0)
.endif
.endif

busyinfo.obj : busyinfo.cpp
calctrl.obj : calctrl.cpp
caret.obj : caret.cpp
choicdgg.obj : choicdgg.cpp
colrdlgg.obj : colrdlgg.cpp
dcpsg.obj : dcpsg.cpp
dirctrlg.obj : dirctrlg.cpp
dirdlgg.obj : dirdlgg.cpp
filedlgg.obj : filedlgg.cpp
fontdlgg.obj : fontdlgg.cpp
grid.obj : grid.cpp
helpext.obj : helpext.cpp
helphtml.obj : helphtml.cpp
helpwxht.obj : helpwxht.cpp
helpxlp.obj : helpxlp.cpp
imaglist.obj : imaglist.cpp
laywin.obj : laywin.cpp
listctrl.obj : listctrl.cpp
logg.obj : logg.cpp
msgdlgg.obj : msgdlgg.cpp
notebook.obj : notebook.cpp
numdlgg.obj : numdlgg.cpp
panelg.obj : panelg.cpp
plot.obj : plot.cpp
printps.obj : printps.cpp
prntdlgg.obj : prntdlgg.cpp
progdlgg.obj : progdlgg.cpp
prop.obj : prop.cpp
propform.obj : propform.cpp
proplist.obj : proplist.cpp
sashwin.obj : sashwin.cpp
scrolwin.obj : scrolwin.cpp
splitter.obj : splitter.cpp
statline.obj : statline.cpp
statusbr.obj : statusbr.cpp
tbarsmpl.obj : tbarsmpl.cpp
tabg.obj : tabg.cpp
textdlgg.obj : textdlgg.cpp
tipdlg.obj : tipdlg.cpp
treectlg.obj : treectlg.cpp
wizard.obj : wizard.cpp

#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 9 November 1999                                                     *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]
	set command $disk2:[joukj.com]bison.cld

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)
.else
.ifdef __WXGTK__
.ifdef __WXUNIVERSAL__
CXX_DEFINE = /define=(__WXGTK__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/ieee=denorm/assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/ieee=denorm
.else
CXX_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm
.endif
.else
CXX_DEFINE =
CC_DEFINE =
.endif
.endif

YACC=bison/yacc

SED=gsed

LEX=flex

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CC_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS = \
		parser.obj,\
		appcmn.obj,\
		artprov.obj,\
		artstd.obj,\
		choiccmn.obj,\
		clipcmn.obj,\
		clntdata.obj,\
		cmdline.obj,\
		cmdproc.obj,\
		cmndata.obj,\
		config.obj,\
		containr.obj,\
		cshelp.obj,\
		ctrlcmn.obj,\
		ctrlsub.obj,\
		datetime.obj,\
		datstrm.obj,\
		db.obj,\
		dbgrid.obj,\
		dbtable.obj,\
		dcbase.obj,\
		dircmn.obj,\
		dlgcmn.obj,\
		dobjcmn.obj,\
		docmdi.obj,\
		docview.obj,\
		dynarray.obj,\
		dynlib.obj,\
		encconv.obj,\
		event.obj,\
		extended.obj,\
		ffile.obj,\
		file.obj,\
		fileconf.obj,\
		filename.obj,\
		filefn.obj,\
		filesys.obj,\
		fontcmn.obj,\
		fontmap.obj,\
		framecmn.obj

OBJECTS1=fs_inet.obj,\
		fs_zip.obj,\
		ftp.obj,\
		gdicmn.obj,\
		gifdecod.obj,\
		hash.obj,\
		hashmap.obj,\
		helpbase.obj,\
		http.obj,\
		iconbndl.obj,\
		imagall.obj,\
		imagbmp.obj,\
		image.obj,\
		imagfill.obj,\
		imaggif.obj,\
		imagiff.obj,\
		imagjpeg.obj,\
		imagpcx.obj,\
		imagpng.obj,\
		imagpnm.obj,\
		imagtiff.obj,\
		imagxpm.obj,\
		intl.obj,\
		ipcbase.obj,\
		layout.obj,\
		lboxcmn.obj,\
		list.obj,\
		log.obj,\
		longlong.obj,\
		memory.obj,\
		menucmn.obj,\
		mimecmn.obj,\
		module.obj,\
		msgout.obj,\
		mstream.obj,\
		nbkbase.obj,\
		object.obj,\
		paper.obj,\
		popupcmn.obj,\
		prntbase.obj,\
		process.obj,\
		protocol.obj,\
		quantize.obj,\
		resource.obj,\
		sckaddr.obj,\
		sckfile.obj,\
		sckipc.obj,\
		sckstrm.obj,\
		sizer.obj,\
		socket.obj,\
		settcmn.obj,\
		statbar.obj,\
		strconv.obj,\
		stream.obj,\
		string.obj,\
		sysopt.obj,\
		tbarbase.obj,\
		textbuf.obj,\
		textcmn.obj,\
		textfile.obj,\
		timercmn.obj,\
		tokenzr.obj,\
		toplvcmn.obj,\
		treebase.obj,\
		txtstrm.obj,\
		unzip.obj,\
		url.obj

OBJECTS2=utilscmn.obj,\
		valgen.obj,\
		validate.obj,\
		valtext.obj,\
		variant.obj,\
		wfstream.obj,\
		wincmn.obj,\
		wxchar.obj,\
		wxexpr.obj,\
		xpmdecod.obj,\
		zipstrm.obj,\
		zstream.obj

SOURCES = \
		parser.y,\
		appcmn.cpp,\
		artprov.cpp,\
		artstd.cpp,\
		choiccmn.cpp,\
		clipcmn.cpp,\
		clntdata.cpp,\
		cmdline.cpp,\
		cmdproc.cpp,\
		cmndata.cpp,\
		config.cpp,\
		containr.cpp,\
		cshelp.cpp,\
		ctrlcmn.cpp,\
		ctrlsub.cpp,\
		datetime.cpp,\
		datstrm.cpp,\
		db.cpp,\
		dbgrid.cpp,\
		dbtable.cpp,\
		dcbase.cpp,\
		dircmn.cpp,\
		dlgcmn.cpp,\
		dobjcmn.cpp,\
		docmdi.cpp,\
		docview.cpp,\
		dynarray.cpp,\
		dynlib.cpp,\
		encconv.cpp,\
		event.cpp,\
		extended.c,\
		ffile.cpp,\
		file.cpp,\
		fileconf.cpp,\
		filename.cpp,\
		filefn.cpp,\
		filesys.cpp,\
		fontcmn.cpp,\
		fontmap.cpp,\
		framecmn.cpp,\
		fs_inet.cpp,\
		fs_zip.cpp,\
		ftp.cpp,\
		gdicmn.cpp,\
		gifdecod.cpp,\
		hash.cpp,\
		hashmap.cpp,\
		helpbase.cpp,\
		http.cpp,\
		iconbndl.cpp,\
		imagall.cpp,\
		imagbmp.cpp,\
		image.cpp,\
		imagfill.cpp,\
		imaggif.cpp,\
		imagiff.cpp,\
		imagjpeg.cpp,\
		imagpcx.cpp,\
		imagpng.cpp,\
		imagpnm.cpp,\
		imagtiff.cpp,\
		imagxpm.cpp,\
		intl.cpp,\
		ipcbase.cpp,\
		layout.cpp,\
		lboxcmn.cpp,\
		list.cpp,\
		log.cpp,\
		longlong.cpp,\
		memory.cpp,\
		menucmn.cpp,\
		mimecmn.cpp,\
		module.cpp,\
		msgout.cpp,\
		mstream.cpp,\
		nbkbase.cpp,\
		object.cpp,\
		paper.cpp,\
		popupcmn.cpp,\
		prntbase.cpp,\
		process.cpp,\
		protocol.cpp,\
		quantize.cpp,\
		resource.cpp,\
		sckaddr.cpp,\
		sckfile.cpp,\
		sckipc.cpp,\
		sckstrm.cpp,\
		sizer.cpp,\
		socket.cpp,\
		settcmn.cpp,\
		statbar.cpp,\
		strconv.cpp,\
		stream.cpp,\
		sysopt.cpp,\
		string.cpp,\
		tbarbase.cpp,\
		textbuf.cpp,\
		textcmn.cpp,\
		textfile.cpp,\
		timercmn.cpp,\
		tokenzr.cpp,\
		toplvcmn.cpp,\
		treebase.cpp,\
		txtstrm.cpp,\
		unzip.c,\
		url.cpp,\
		utilscmn.cpp,\
		valgen.cpp,\
		validate.cpp,\
		valtext.cpp,\
		variant.cpp,\
		wfstream.cpp,\
		wincmn.cpp,\
		wxchar.cpp,\
		wxexpr.cpp,\
		xpmdecod.cpp,\
		zipstrm.cpp,\
		zstream.cpp

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS1)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS2)
.ifdef __WXMOTIF__
	library [--.lib]libwx_motif.olb $(OBJECTS)
	library [--.lib]libwx_motif.olb $(OBJECTS1)
	library [--.lib]libwx_motif.olb $(OBJECTS2)
.else
.ifdef __WXGTK__
.ifdef __WXUNIVERSAL__
	library [--.lib]libwx_gtk_univ.olb $(OBJECTS)
	library [--.lib]libwx_gtk_univ.olb $(OBJECTS1)
	library [--.lib]libwx_gtk_univ.olb $(OBJECTS2)
.else
	library [--.lib]libwx_gtk.olb $(OBJECTS)
	library [--.lib]libwx_gtk.olb $(OBJECTS1)
	library [--.lib]libwx_gtk.olb $(OBJECTS2)
.endif
.endif
.endif

parser.obj : parser.c lexer.c
parser.c : parser.y lexer.c
	$(YACC) parser.y
	pipe $(SED) -e "s;y_tab.c;parser.y;g" < y_tab.c | \
	$(SED) -e "s/BUFSIZ/5000/g"            | \
	$(SED) -e "s/YYLMAX 200/YYLMAX 5000/g" | \
	$(SED) -e "s/yy/PROIO_yy/g"            | \
	$(SED) -e "s/input/PROIO_input/g"      | \
	$(SED) -e "s/unput/PROIO_unput/g"      > parser.c
	delete y_tab.c;*

lexer.c : lexer.l
	$(LEX) lexer.l
	pipe $(SED) -e "s;lexyy.c;lexer.l;g" < lexyy.c | \
	$(SED) -e "s/yy/PROIO_yy/g"            | \
	$(SED) -e "s/input/PROIO_input/g"      | \
	$(SED) -e "s/unput/PROIO_unput/g"      > lexer.c
	delete lexyy.c;*

appcmn.obj : appcmn.cpp
artprov.obj : artprov.cpp
artstd.obj : artstd.cpp
choiccmn.obj : choiccmn.cpp
clipcmn.obj : clipcmn.cpp
clntdata.obj : clntdata.cpp
cmdline.obj : cmdline.cpp
cmdproc.obj : cmdproc.cpp
cmndata.obj : cmndata.cpp
config.obj : config.cpp
containr.obj : containr.cpp
cshelp.obj : cshelp.cpp
ctrlcmn.obj : ctrlcmn.cpp
ctrlsub.obj : ctrlsub.cpp
datetime.obj : datetime.cpp
datstrm.obj : datstrm.cpp
db.obj : db.cpp
dbgrid.obj : dbgrid.cpp
dbtable.obj : dbtable.cpp
dcbase.obj : dcbase.cpp
dircmn.obj : dircmn.cpp
dlgcmn.obj : dlgcmn.cpp
dobjcmn.obj : dobjcmn.cpp
docmdi.obj : docmdi.cpp
docview.obj : docview.cpp
dynarray.obj : dynarray.cpp
dynlib.obj : dynlib.cpp
encconv.obj : encconv.cpp
event.obj : event.cpp
extended.obj : extended.c
ffile.obj : ffile.cpp
file.obj : file.cpp
fileconf.obj : fileconf.cpp
filefn.obj : filefn.cpp
filename.obj : filename.cpp
filesys.obj : filesys.cpp
fontcmn.obj : fontcmn.cpp
fontmap.obj : fontmap.cpp
framecmn.obj : framecmn.cpp
fs_inet.obj : fs_inet.cpp
fs_zip.obj : fs_zip.cpp
ftp.obj : ftp.cpp
gdicmn.obj : gdicmn.cpp
gifdecod.obj : gifdecod.cpp
hash.obj : hash.cpp
hashmap.obj : hashmap.cpp
helpbase.obj : helpbase.cpp
http.obj : http.cpp
iconbndl.obj : iconbndl.cpp
imagall.obj : imagall.cpp
imagbmp.obj : imagbmp.cpp
image.obj : image.cpp
imagfill.obj : imagfill.cpp
imaggif.obj : imaggif.cpp
imagiff.obj : imagiff.cpp
imagjpeg.obj : imagjpeg.cpp
imagpcx.obj : imagpcx.cpp
imagpng.obj : imagpng.cpp
imagpnm.obj : imagpnm.cpp
imagtiff.obj : imagtiff.cpp
imagxpm.obj : imagxpm.cpp
intl.obj : intl.cpp
ipcbase.obj : ipcbase.cpp
layout.obj : layout.cpp
lboxcmn.obj : lboxcmn.cpp
list.obj : list.cpp
log.obj : log.cpp
longlong.obj : longlong.cpp
memory.obj : memory.cpp
menucmn.obj : menucmn.cpp
mimecmn.obj : mimecmn.cpp
module.obj : module.cpp
msgout.obj : msgout.cpp
mstream.obj : mstream.cpp
nbkbase.obj : nbkbase.cpp
object.obj : object.cpp
paper.obj : paper.cpp
popupcmn.obj : popupcmn.cpp
prntbase.obj : prntbase.cpp
process.obj : process.cpp
protocol.obj : protocol.cpp
quantize.obj : quantize.cpp
resource.obj : resource.cpp
sckaddr.obj : sckaddr.cpp
sckfile.obj : sckfile.cpp
sckipc.obj : sckipc.cpp
sckstrm.obj : sckstrm.cpp
sizer.obj : sizer.cpp
socket.obj : socket.cpp
settcmn.obj : settcmn.cpp
statbar.obj : statbar.cpp
strconv.obj : strconv.cpp
stream.obj : stream.cpp
sysopt.obj : sysopt.cpp
string.obj : string.cpp
tbarbase.obj : tbarbase.cpp
textbuf.obj : textbuf.cpp
textcmn.obj : textcmn.cpp
textfile.obj : textfile.cpp
timercmn.obj : timercmn.cpp
tokenzr.obj : tokenzr.cpp
toplvcmn.obj : toplvcmn.cpp
treebase.obj : treebase.cpp
txtstrm.obj : txtstrm.cpp
unzip.obj : unzip.c
url.obj : url.cpp
utilscmn.obj : utilscmn.cpp
valgen.obj : valgen.cpp
validate.obj : validate.cpp
valtext.obj : valtext.cpp
variant.obj : variant.cpp
wfstream.obj : wfstream.cpp
wincmn.obj : wincmn.cpp
wxchar.obj : wxchar.cpp
wxexpr.obj : wxexpr.cpp
xpmdecod.obj : xpmdecod.cpp
zipstrm.obj : zipstrm.cpp
zstream.obj : zstream.cpp

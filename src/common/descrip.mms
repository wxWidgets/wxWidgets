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
CXX_DEFINE = /define=(__WXMOTIF__=1)
.else
CXX_DEFINE =
.endif

YACC=bison/yacc

SED=gsed

LEX=flex

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS = \
		parser.obj,\
		appcmn.obj,\
		choiccmn.obj,\
		clipcmn.obj,\
		cmndata.obj,\
		config.obj,\
		ctrlcmn.obj,\
		ctrlsub.obj,\
		datetime.obj,\
		datstrm.obj,\
		db.obj,\
		dbtable.obj,\
		dcbase.obj,\
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
		filefn.obj,\
		filesys.obj,\
		fontcmn.obj,\
		fontmap.obj

OBJECTS1=framecmn.obj,\
		fs_inet.obj,\
		fs_zip.obj,\
		ftp.obj,\
		gdicmn.obj,\
		gifdecod.obj,\
		hash.obj,\
		helpbase.obj,\
		http.obj,\
		imagall.obj,\
		imagbmp.obj,\
		image.obj,\
		imaggif.obj,\
		imagjpeg.obj,\
		imagpcx.obj,\
		imagpng.obj,\
		imagpnm.obj,\
		imagtiff.obj,\
		intl.obj,\
		ipcbase.obj,\
		layout.obj,\
		lboxcmn.obj,\
		list.obj,\
		log.obj,\
		longlong.obj,\
		memory.obj,\
		menucmn.obj,\
		mimetype.obj,\
		module.obj,\
		mstream.obj,\
		object.obj,\
		objstrm.obj,\
		paper.obj,\
		prntbase.obj,\
		process.obj,\
		protocol.obj,\
		resource.obj,\
		sckaddr.obj,\
		sckfile.obj,\
		sckipc.obj,\
		sckstrm.obj,\
		serbase.obj,\
		sizer.obj,\
		socket.obj,\
		strconv.obj,\
		stream.obj,\
		string.obj,\
		tbarbase.obj,\
		textcmn.obj,\
		textfile.obj,\
		timercmn.obj,\
		tokenzr.obj,\
		txtstrm.obj,\
		unzip.obj,\
		url.obj,\
		utilscmn.obj,\
		valgen.obj,\
		validate.obj,\
		valtext.obj,\
		variant.obj,\
		wfstream.obj,\
		wincmn.obj,\
		wxchar.obj,\
		wxexpr.obj,\
		zipstrm.obj,\
		zstream.obj

SOURCES = \
		parser.y,\
		appcmn.cpp,\
		choiccmn.cpp,\
		clipcmn.cpp,\
		cmndata.cpp,\
		config.cpp,\
		ctrlcmn.cpp,\
		ctrlsub.cpp,\
		datetime.cpp,\
		datstrm.cpp,\
		db.cpp,\
		dbtable.cpp,\
		dcbase.cpp,\
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
		helpbase.cpp,\
		http.cpp,\
		imagall.cpp,\
		imagbmp.cpp,\
		image.cpp,\
		imaggif.cpp,\
		imagjpeg.cpp,\
		imagpcx.cpp,\
		imagpng.cpp,\
		imagpnm.cpp,\
		imagtiff.cpp,\
		intl.cpp,\
		ipcbase.cpp,\
		layout.cpp,\
		lboxcmn.cpp,\
		list.cpp,\
		log.cpp,\
		longlong.cpp,\
		memory.cpp,\
		menucmn.cpp,\
		mimetype.cpp,\
		module.cpp,\
		mstream.cpp,\
		object.cpp,\
		objstrm.cpp,\
		paper.cpp,\
		prntbase.cpp,\
		process.cpp,\
		protocol.cpp,\
		resource.cpp,\
		sckaddr.cpp,\
		sckfile.cpp,\
		sckipc.cpp,\
		sckstrm.cpp,\
		serbase.cpp,\
		sizer.cpp,\
		socket.cpp,\
		strconv.cpp,\
		stream.cpp,\
		string.cpp,\
		tbarbase.cpp,\
		textcmn.cpp,\
		textfile.cpp,\
		timercmn.cpp,\
		tokenzr.cpp,\
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
		zipstrm.cpp,\
		zstream.cpp

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS1)
.ifdef __WXMOTIF__
	library [--.lib]libwx_motif.olb $(OBJECTS)
	library [--.lib]libwx_motif.olb $(OBJECTS1)
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
choiccmn.obj : choiccmn.cpp
clipcmn.obj : clipcmn.cpp
cmndata.obj : cmndata.cpp
config.obj : config.cpp
ctrlcmn.obj : ctrlcmn.cpp
ctrlsub.obj : ctrlsub.cpp
datetime.obj : datetime.cpp
datstrm.obj : datstrm.cpp
db.obj : db.cpp
dbtable.obj : dbtable.cpp
dcbase.obj : dcbase.cpp
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
helpbase.obj : helpbase.cpp
http.obj : http.cpp
imagall.obj : imagall.cpp
imagbmp.obj : imagbmp.cpp
image.obj : image.cpp
imaggif.obj : imaggif.cpp
imagjpeg.obj : imagjpeg.cpp
imagpcx.obj : imagpcx.cpp
imagpng.obj : imagpng.cpp
imagpnm.obj : imagpnm.cpp
imagtiff.obj : imagtiff.cpp
intl.obj : intl.cpp
ipcbase.obj : ipcbase.cpp
layout.obj : layout.cpp
lboxcmn.obj : lboxcmn.cpp
list.obj : list.cpp
log.obj : log.cpp
longlong.obj : longlong.cpp
memory.obj : memory.cpp
menucmn.obj : menucmn.cpp
mimetype.obj : mimetype.cpp
module.obj : module.cpp
mstream.obj : mstream.cpp
object.obj : object.cpp
objstrm.obj : objstrm.cpp
paper.obj : paper.cpp
prntbase.obj : prntbase.cpp
process.obj : process.cpp
protocol.obj : protocol.cpp
resource.obj : resource.cpp
sckaddr.obj : sckaddr.cpp
sckfile.obj : sckfile.cpp
sckipc.obj : sckipc.cpp
sckstrm.obj : sckstrm.cpp
serbase.obj : serbase.cpp
sizer.obj : sizer.cpp
socket.obj : socket.cpp
strconv.obj : strconv.cpp
stream.obj : stream.cpp
string.obj : string.cpp
tbarbase.obj : tbarbase.cpp
textcmn.obj : textcmn.cpp
textfile.obj : textfile.cpp
timercmn.obj : timercmn.cpp
tokenzr.obj : tokenzr.cpp
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
zipstrm.obj : zipstrm.cpp
zstream.obj : zstream.cpp

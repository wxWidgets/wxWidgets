#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 13 February 2006                                                    *
#                                                                            *
#*****************************************************************************
.first
	define wx [---.include.wx]
	set command $disk2:[joukj.com]bison.cld

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)
.else
.ifdef __WXGTK2__
CXX_DEFINE = /define=(__WXGTK__=1,VMS_GTK2=1)/float=ieee\
	/name=(as_is,short)/ieee=denorm/assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1,VMS_GTK2=1)/float=ieee\
	/name=(as_is,short)/ieee=denorm
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
.endif

YACC=bison/yacc

SED=gsed

LEX=flex

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CC_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS=prop.obj,propform.obj,proplist.obj,wxexpr.obj,parser.obj,resource.obj,\
	treelay.obj

SOURCES=prop.cpp,propform.cpp,proplist.cpp,wxexpr.cpp,parser.y,resource.cpp,\
	treelay.cpp

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
.ifdef __WXMOTIF__
	library/crea [---.lib]libwx_motif_deprecated.olb $(OBJECTS)
.else
.ifdef __WXGTK2__
	library/crea [---.lib]libwx_gtk2_deprecated.olb $(OBJECTS)
.else
.ifdef __WXGTK__
.ifdef __WXUNIVERSAL__
	library/crea [---.lib]libwx_gtk_univ_deprecated.olb $(OBJECTS)
.else
	library/crea [---.lib]libwx_gtk_deprecated.olb $(OBJECTS)
.endif
.endif
.endif
.endif

prop.obj : prop.cpp
propform.obj : propform.cpp
proplist.obj : proplist.cpp
wxexpr.obj : wxexpr.cpp
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

resource.obj : resource.cpp
treelay.obj : treelay.cpp

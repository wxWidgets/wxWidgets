#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 20 October 2004                                                      *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm
.else
.ifdef __WXX11__
CXX_DEFINE = /define=(__WXX11__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXX11__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)
.else
CXX_DEFINE =
CC_DEFINE =
.endif
.endif
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CC_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS=xh_bmp.obj,xh_bmpbt.obj,xh_bttn.obj,xh_cald.obj,xh_chckb.obj,\
	xh_chckl.obj,xh_choic.obj,xh_combo.obj,xh_dlg.obj,xh_frame.obj,\
	xh_gauge.obj,xh_gdctl.obj,xh_html.obj,xh_listb.obj,xh_listc.obj,\
	xh_menu.obj,xh_notbk.obj,xh_panel.obj,xh_radbt.obj,xh_radbx.obj,\
	xh_scrol.obj,xh_scwin.obj,xh_sizer.obj,xh_slidr.obj,xh_spin.obj,\
	xh_split.obj,xh_statbar.obj,xh_stbmp.obj,xh_stbox.obj,xh_stlin.obj,\
	xh_sttxt.obj,xh_text.obj,xh_tglbtn.obj,xh_toolb.obj,xh_tree.obj,\
	xh_unkwn.obj,xh_wizrd.obj,xmlres.obj,xmlrsall.obj     

SOURCES =xh_bmp.cpp,xh_bmpbt.cpp,xh_bttn.cpp,xh_cald.cpp,xh_chckb.cpp,\
	xh_chckl.cpp,xh_choic.cpp,xh_combo.cpp,xh_dlg.cpp,xh_frame.cpp,\
	xh_gauge.cpp,xh_gdctl.cpp,xh_html.cpp,xh_listb.cpp,xh_listc.cpp,\
	xh_menu.cpp,xh_notbk.cpp,xh_panel.cpp,xh_radbt.cpp,xh_radbx.cpp,\
	xh_scrol.cpp,xh_scwin.cpp,xh_sizer.cpp,xh_slidr.cpp,xh_spin.cpp,\
	xh_split.cpp,xh_statbar.cpp,xh_stbmp.cpp,xh_stbox.cpp,xh_stlin.cpp,\
	xh_sttxt.cpp,xh_text.cpp,xh_tglbtn.cpp,xh_toolb.cpp,xh_tree.cpp,\
	xh_unkwn.cpp,xh_wizrd.cpp,xmlres.cpp,xmlrsall.cpp     
  
all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
.ifdef __WXMOTIF__
	library [--.lib]libwx_motif.olb $(OBJECTS)
.else
.ifdef __WXGTK__
	library [--.lib]libwx_gtk.olb $(OBJECTS)
.else
.ifdef __WXX11__
	library [--.lib]libwx_x11_univ.olb $(OBJECTS)
.endif
.endif
.endif

xh_bmp.obj : xh_bmp.cpp
xh_bmpbt.obj : xh_bmpbt.cpp
xh_bttn.obj : xh_bttn.cpp
xh_cald.obj : xh_cald.cpp
xh_chckb.obj : xh_chckb.cpp
xh_chckl.obj : xh_chckl.cpp
xh_choic.obj : xh_choic.cpp
xh_combo.obj : xh_combo.cpp
xh_dlg.obj : xh_dlg.cpp
xh_frame.obj : xh_frame.cpp
xh_gauge.obj : xh_gauge.cpp
xh_gdctl.obj : xh_gdctl.cpp
xh_html.obj : xh_html.cpp
xh_listb.obj : xh_listb.cpp
xh_listc.obj : xh_listc.cpp
xh_menu.obj : xh_menu.cpp
xh_notbk.obj : xh_notbk.cpp
xh_panel.obj : xh_panel.cpp
xh_radbt.obj : xh_radbt.cpp
xh_radbx.obj : xh_radbx.cpp
xh_scrol.obj : xh_scrol.cpp
xh_scwin.obj : xh_scwin.cpp
xh_sizer.obj : xh_sizer.cpp
xh_slidr.obj : xh_slidr.cpp
xh_spin.obj : xh_spin.cpp
xh_split.obj : xh_split.cpp
xh_statbar.obj : xh_statbar.cpp
xh_stbmp.obj : xh_stbmp.cpp
xh_stbox.obj : xh_stbox.cpp
xh_stlin.obj : xh_stlin.cpp
xh_sttxt.obj : xh_sttxt.cpp
xh_text.obj : xh_text.cpp
xh_tglbtn.obj : xh_tglbtn.cpp
xh_toolb.obj : xh_toolb.cpp
xh_tree.obj : xh_tree.cpp
xh_unkwn.obj : xh_unkwn.cpp
xh_wizrd.obj : xh_wizrd.cpp
xmlres.obj : xmlres.cpp
xmlrsall.obj : xmlrsall.cpp

# wxXML makefile

WXDIR = ..\..\..

#XMLDIR=
!error modify makefile.wat to include the path to the XML library

EXTRACPPFLAGS = -I$(WXDIR)\contrib\include;-I$(XMLDIR)\libxml2-2.1.1

!include $(WXDIR)\src\makewat.env

LIBTARGET = $(WXDIR)\lib\wxxrc_w.lib

OBJECTS=&
    $(OUTPUTDIR)\xml.obj &
    $(OUTPUTDIR)\xmlres.obj &
    $(OUTPUTDIR)\xmlrsall.obj &
    $(OUTPUTDIR)\xh_bttn.obj &
    $(OUTPUTDIR)\xh_chckb.obj &
    $(OUTPUTDIR)\xh_chckl.obj &
    $(OUTPUTDIR)\xh_choic.obj &
    $(OUTPUTDIR)\xh_combo.obj &
    $(OUTPUTDIR)\xh_dlg.obj &
    $(OUTPUTDIR)\xh_gauge.obj &
    $(OUTPUTDIR)\xh_html.obj &
    $(OUTPUTDIR)\xh_menu.obj &
    $(OUTPUTDIR)\xh_notbk.obj &
    $(OUTPUTDIR)\xh_panel.obj &
    $(OUTPUTDIR)\xh_radbt.obj &
    $(OUTPUTDIR)\xh_radbx.obj &
    $(OUTPUTDIR)\xh_sizer.obj &
    $(OUTPUTDIR)\xh_slidr.obj &
    $(OUTPUTDIR)\xh_spin.obj &
    $(OUTPUTDIR)\xh_stbmp.obj &
    $(OUTPUTDIR)\xh_sttxt.obj &
    $(OUTPUTDIR)\xh_text.obj &
    $(OUTPUTDIR)\xh_listb.obj&
    $(OUTPUTDIR)\xh_toolb.obj &
    $(OUTPUTDIR)\xh_stlin.obj &
    $(OUTPUTDIR)\xh_bmp.obj &
    $(OUTPUTDIR)\xh_bmpbt.obj &
    $(OUTPUTDIR)\xh_cald.obj &
    $(OUTPUTDIR)\xh_listc.obj &
    $(OUTPUTDIR)\xh_scrol.obj &
    $(OUTPUTDIR)\xh_stbox.obj &
    $(OUTPUTDIR)\xh_tree.obj &
    $(OUTPUTDIR)\xh_unkwn.obj &
    $(OUTPUTDIR)\xh_frame.obj &
    $(OUTPUTDIR)\xh_scwin.obj &
    $(OUTPUTDIR)\xh_split.obj &
    $(OUTPUTDIR)\xh_wizrd.obj

!include $(WXDIR)\src\makelib.wat

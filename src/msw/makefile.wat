#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	Edward C. Zimmermann <edz@crg.net>
# Created:	1994
# Updated:	Dmitri Chubraev, Nov.1994 <chubraev@iem.ee.ethz.ch>
# RCS_ID        $Id$
#
# Makefile : Builds wxWindows library for Windows 3.1
# and Watcom C++ 

WXDIR = ..\..

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib

LIBTARGET   = $(WXLIB)\wx$(LEVEL).lib
DUMMY=dummydll
#CTL3DOBJ    = ..\..\contrib\ctl3d\ctl3d32.obj
#CTL3DLIB    = ..\..\contrib\ctl3d\win32s\ctl3d32.lib
FAFALIB     = ..\..\contrib\fafa\fafa.lib
#ODBCLIB     = ..\..\contrib\odbc\odbc32.lib
GAUGELIB    = ..\..\contrib\gauge\gauge.lib
ITSYLIB     = ..\..\contrib\itsybits\itsy.lib
WXSTRINGLIB = ..\..\contrib\wxstring\wxstring.lib
WXXPMLIB    = ..\..\contrib\wxxpm\wxxpm.lib
PROIOLIB    = ..\..\utils\prologio\lib\prologio.lib
DIBLIB      = ..\..\utils\dib\dib.lib
RCPARSERLIB = ..\..\utils\rcparser\lib\rcparser.lib

EXTRAMODULES = $(GAUGELIB) $(ITSYLIB) $(PROIOLIB) $(DIBLIB) $(WXSTRINGLIB) $(RCPARSERLIB) $(FAFALIB) # $(WXXPMLIB)
EXTRATARGETS = fafa gauge itsy prologio dib rcparser wxstring # wxxpm
EXTRATARGETSCLEAN = clean_fafa clean_gauge clean_itsy clean_proio clean_dib clean_rcp clean_wxstring # clean_wxxpm

OBJECTS = wx_win.obj wx_frame.obj wx_panel.obj wx_utils.obj &
  wx_item.obj wx_text.obj wx_gdi.obj wx_dialg.obj wx_canvs.obj wx_dc.obj &
  wx_mf.obj wx_ipc.obj wx_timer.obj wx_clipb.obj wx_vlbox.obj &
  wx_stat.obj wx_scrol.obj wx_buttn.obj wx_messg.obj wx_check.obj wx_choic.obj &
  wx_rbox.obj wx_lbox.obj wx_group.obj wx_gauge.obj wx_txt.obj wx_mtxt.obj &
  wx_slidr.obj wx_menu.obj wx_db.obj wx_cmdlg.obj wx_main.obj wx_combo.obj

BASEOBJECTS = ..\base\wb_win.obj ..\base\wb_frame.obj ..\base\wb_panel.obj &
	..\base\wb_utils.obj ..\base\wx_lay.obj ..\base\wx_doc.obj ..\base\wb_res.obj &
        ..\base\wb_main.obj ..\base\wb_item.obj ..\base\wb_list.obj ..\base\wb_obj.obj &
        ..\base\wb_text.obj ..\base\wb_gdi.obj ..\base\wb_dialg.obj ..\base\wb_canvs.obj &
        ..\base\wb_dc.obj ..\base\wb_mf.obj ..\base\wb_ps.obj ..\base\wx_enhdg.obj &
        ..\base\wb_hash.obj ..\base\wb_ipc.obj ..\base\wb_form.obj ..\base\wb_timer.obj &
        ..\base\wb_help.obj ..\base\wb_sysev.obj ..\base\wb_stdev.obj ..\base\wb_types.obj &
        ..\base\wb_mgstr.obj ..\base\wb_data.obj ..\base\wb_stat.obj &
        ..\base\wb_scrol.obj ..\base\wb_vlbox.obj ..\base\wb_print.obj ..\base\wx_tbar.obj &
        ..\base\wx_bbar.obj ..\base\wx_mem.obj ..\base\wx_date.obj ..\base\wb_cmdlg.obj &
        ..\base\wx_time.obj ..\base\wx_frac.obj

# This now replaced by contrib\wxstring
#..\base\wxstring.obj 

all:        base $(EXTRATARGETS) erasepch $(LIBTARGET)

base:   .SYMBOLIC
    cd ..\base
    wmake -f makefile.wat all OPTIONS="$(OPTIONS)" DEBUG="$(DEBUG)" LEVEL=$(LEVEL)
    cd ..\msw

$(LIBTARGET) : $(OBJECTS) $(BASEOBJECTS) $(EXTRAMODULES) 
    %create tmp.lbc
    @for %i in ( $(OBJECTS) ) do @%append tmp.lbc +%i
    @for %i in ( $(BASEOBJECTS) ) do @%append tmp.lbc +%i
    @for %i in ( $(EXTRAMODULES) ) do @%append tmp.lbc +%i
    wlib /b /c /n /p=512 $^@ @tmp.lbc
	
clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch
     cd ..\base
     wmake -f makefile.wat clean
    -erase $(LIBTARGET)
     cd ..\msw

cleanall:   clean $(EXTRATARGETSCLEAN)

fafa:   .SYMBOLIC
    cd $(WXDIR)\contrib\fafa
    wmake -f makefile.wat all
    cd $(WXDIR)\src\msw

clean_fafa:   .SYMBOLIC
    cd $(WXDIR)\contrib\fafa
    wmake -f makefile.wat clean
    cd $(WXDIR)\src\msw

itsy:   .SYMBOLIC
    cd $(WXDIR)\contrib\itsybits
    wmake -f makefile.wat all
    cd $(WXDIR)\src\msw

clean_itsy:   .SYMBOLIC
    cd $(WXDIR)\contrib\itsybits
    wmake -f makefile.wat clean
    cd $(WXDIR)\src\msw

gauge:   .SYMBOLIC
    cd $(WXDIR)\contrib\gauge
    wmake -f makefile.wat all
    cd $(WXDIR)\src\msw

clean_gauge:   .SYMBOLIC
    cd $(WXDIR)\contrib\gauge
    wmake -f makefile.wat clean
    cd $(WXDIR)\src\msw

wxxpm:   .SYMBOLIC
    cd $(WXDIR)\contrib\wxxpm
    wmake -f makefile.wat all
    cd $(WXDIR)\src\msw

clean_wxxpm:   .SYMBOLIC
    cd $(WXDIR)\contrib\wxxpm
    wmake -f makefile.wat clean
    cd $(WXDIR)\src\msw

dib:   .SYMBOLIC
    cd $(WXDIR)\utils\dib
    wmake -f makefile.wat all
    cd $(WXDIR)\src\msw

clean_dib:   .SYMBOLIC
    cd $(WXDIR)\utils\dib
    wmake -f makefile.wat clean
    cd $(WXDIR)\src\msw

prologio:   .SYMBOLIC
    cd $(WXDIR)\utils\prologio\src
    wmake -f makefile.wat all
    cd $(WXDIR)\src\msw

clean_proio:   .SYMBOLIC
    cd $(WXDIR)\utils\prologio\src
    wmake -f makefile.wat clean
    cd $(WXDIR)\src\msw

rcparser:   .SYMBOLIC
    cd $(WXDIR)\utils\rcparser\src
    wmake -f makefile.wat all
    cd $(WXDIR)\src\msw

wxstring:   .SYMBOLIC
    cd $(WXDIR)\contrib\wxstring
    wmake -f makefile.wat all OPTIONS="$(OPTIONS)" DEBUG="$(DEBUG)"
    cd $(WXDIR)\src\msw

clean_wxstring:   .SYMBOLIC
    cd $(WXDIR)\contrib\wxstring
    wmake -f makefile.wat clean
    cd $(WXDIR)\src\msw

clean_rcp:   .SYMBOLIC
    cd $(WXDIR)\utils\rcparser\src
    wmake -f makefile.wat clean
    cd $(WXDIR)\src\msw

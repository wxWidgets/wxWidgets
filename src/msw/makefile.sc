# Symantec C++ makefile for the msw objects
# called from src\makefile.sc

# configuration section (see src\makefile.sc) ###########################

WXDIR = $(WXWIN)

!include $(WXDIR)\src\makesc.env

INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

# default values overridden by src\makefile.sc

CC=sc
CFLAGS = -o -ml -W -Dwx_msw

INCLUDE=$(BASEINC);$(MSWINC);$(WXDIR)\contrib\fafa;$(WXDIR)\contrib\itsybits

OPTIONS=

# end of configuration section ##########################################

OBJS = wx_win.obj wx_frame.obj wx_panel.obj wx_utils.obj wx_main.obj \
wx_item.obj wx_text.obj wx_gdi.obj wx_dialg.obj wx_canvs.obj wx_dc.obj \
wx_mf.obj wx_ipc.obj wx_timer.obj wx_clipb.obj wx_scrol.obj wx_vlbox.obj \
wx_stat.obj wx_buttn.obj wx_messg.obj wx_check.obj wx_choic.obj wx_rbox.obj wx_lbox.obj \
wx_group.obj wx_gauge.obj wx_txt.obj wx_mtxt.obj wx_slidr.obj wx_menu.obj wx_db.obj\
wx_cmdlg.obj

all: $(OBJS)

wx_obj.obj: $(BASEINC)\wx_obj.h

wx_win.obj: $(BASEINC)\wx_defs.h $(MSWINC)\wx_win.h \
$(BASEINC)\wx_obj.h $(BASEINC)\wx_utils.h wx_win.$(SRCSUFF) \
$(MSWINC)\wx_gdi.h $(MSWINC)\wx_privt.h

wx_main.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wx_obj.h \
$(MSWINC)\wx_frame.h $(BASEINC)\wx_utils.h

wx_frame.obj: $(BASEINC)\wx_defs.h $(MSWINC)\wx_win.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(MSWINC)\wx_frame.h wx_frame.$(SRCSUFF) \
$(BASEINC)\wx_stdev.h $(MSWINC)\wx_privt.h

wx_panel.obj: $(BASEINC)\wx_defs.h $(MSWINC)\wx_win.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(MSWINC)\wx_frame.h $(MSWINC)\wx_panel.h \
wx_panel.$(SRCSUFF) $(BASEINC)\wx_stdev.h $(MSWINC)\wx_privt.h

wx_text.obj: $(BASEINC)\wx_defs.h $(MSWINC)\wx_win.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(MSWINC)\wx_frame.h $(MSWINC)\wx_text.h \
wx_text.$(SRCSUFF) $(BASEINC)\wx_stdev.h $(MSWINC)\wx_privt.h

wx_canvs.obj: $(BASEINC)\wx_defs.h $(MSWINC)\wx_win.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(MSWINC)\wx_frame.h $(MSWINC)\wx_canvs.h \
wx_canvs.$(SRCSUFF) $(BASEINC)\wx_stdev.h $(MSWINC)\wx_gdi.h $(MSWINC)\wx_dc.h \
$(MSWINC)\wx_privt.h

wx_dc.obj: $(BASEINC)\wx_defs.h $(MSWINC)\wx_win.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(MSWINC)\wx_frame.h $(MSWINC)\wx_canvs.h wx_dc.$(SRCSUFF) \
$(BASEINC)\wx_stdev.h $(MSWINC)\wx_gdi.h $(MSWINC)\wx_dc.h \
$(MSWINC)/wx_dccan.h $(MSWINC)/wx_dcmem.h

wx_mf.obj: $(BASEINC)\wx_defs.h $(MSWINC)\wx_win.h $(BASEINC)\wx_obj.h \
wx_mf.$(SRCSUFF) $(BASEINC)\wx_stdev.h $(MSWINC)\wx_gdi.h $(MSWINC)\wx_mf.h

wx_item.obj: $(BASEINC)\wx_defs.h $(MSWINC)\wx_win.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(MSWINC)\wx_frame.h $(MSWINC)\wx_item.h \
wx_item.$(SRCSUFF) $(BASEINC)\wx_stdev.h $(MSWINC)\wx_privt.h

wx_utils.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h wx_utils.$(SRCSUFF)

wx_ipc.obj: $(BASEINC)\wx_defs.h $(BASEINC)\wx_obj.h \
$(BASEINC)\wx_utils.h $(MSWINC)\wx_ipc.h wx_ipc.$(SRCSUFF)

wx_gdi.obj: $(BASEINC)\wx_defs.h $(MSWINC)\wx_gdi.h $(BASEINC)\wx_utils.h \
wx_gdi.$(SRCSUFF)

wx_dialg.obj: $(BASEINC)\wx_defs.h wx_dialg.$(SRCSUFF) $(MSWINC)\wx_dialg.h \
$(MSWINC)\wx_win.h $(BASEINC)\wx_utils.h $(MSWINC)\wx_panel.h \
$(MSWINC)\wx_privt.h

wx_timer.obj: $(BASEINC)\wx_defs.h wx_timer.$(SRCSUFF) $(MSWINC)\wx_timer.h

wx_clipb.obj: $(BASEINC)\wx_defs.h wx_clipb.$(SRCSUFF) $(MSWINC)\wx_clipb.h

wx_stat.obj: wx_stat.$(SRCSUFF) $(MSWINC)\wx_stat.h

wx_scrol.obj: wx_scrol.$(SRCSUFF) $(MSWINC)\wx_scrol.h

wx_vlbox.obj: wx_vlbox.$(SRCSUFF) $(MSWINC)\wx_vlbox.h

wx_buttn.obj: wx_buttn.$(SRCSUFF) $(MSWINC)\wx_buttn.h

wx_messg.obj: wx_messg.$(SRCSUFF) $(MSWINC)\wx_messg.h

wx_check.obj: wx_check.$(SRCSUFF) $(MSWINC)\wx_check.h

wx_choic.obj: wx_choic.$(SRCSUFF) $(MSWINC)\wx_choic.h

wx_rbox.obj: wx_rbox.$(SRCSUFF) $(MSWINC)\wx_rbox.h

wx_lbox.obj: wx_lbox.$(SRCSUFF) $(MSWINC)\wx_lbox.h

wx_group.obj: wx_group.$(SRCSUFF) $(MSWINC)\wx_group.h

wx_gauge.obj: wx_gauge.$(SRCSUFF) $(MSWINC)\wx_gauge.h

wx_txt.obj: wx_txt.$(SRCSUFF) $(MSWINC)\wx_txt.h

wx_mtxt.obj: wx_mtxt.$(SRCSUFF) $(MSWINC)\wx_mtxt.h

wx_slidr.obj: wx_slidr.$(SRCSUFF) $(MSWINC)\wx_slidr.h

wx_menu.obj: wx_menu.$(SRCSUFF) $(MSWINC)\wx_menu.h

wx_db.obj: wx_db.$(SRCSUFF) $(MSWINC)\wx_db.h

wx_cmdlg.obj: wx_cmdlg.$(SRCSUFF) $(MSWINC)\wx_cmdlg.h

$(MSWINC)/wx_win.h:  $(BASEINC)/wb_win.h
$(MSWINC)/wx_main.h:  $(BASEINC)/wb_main.h
$(MSWINC)/wx_frame.h:  $(BASEINC)/wb_frame.h
$(MSWINC)/wx_panel.h:  $(BASEINC)/wb_panel.h
$(MSWINC)/wx_text.h:  $(BASEINC)/wb_text.h
$(MSWINC)/wx_dialg.h:  $(BASEINC)/wb_dialg.h
$(MSWINC)/wx_ipc.h:  $(BASEINC)/wb_ipc.h
$(MSWINC)/wx_gdi.h:  $(BASEINC)/wb_gdi.h
$(MSWINC)/wx_event.h:  $(BASEINC)/wb_event.h
$(MSWINC)/wx_canvs.h:  $(BASEINC)/wb_canvs.h
$(MSWINC)/wx_mf.h:  $(BASEINC)/wb_mf.h
$(MSWINC)/wx_item.h:  $(BASEINC)/wb_item.h
$(MSWINC)/wx_buttn.h:  $(BASEINC)/wb_buttn.h
$(MSWINC)/wx_messg.h:  $(BASEINC)/wb_messg.h
$(MSWINC)/wx_choic.h:  $(BASEINC)/wb_choic.h
$(MSWINC)/wx_check.h:  $(BASEINC)/wb_check.h
$(MSWINC)/wx_lbox.h:  $(BASEINC)/wb_lbox.h
$(MSWINC)/wx_txt.h:  $(BASEINC)/wb_txt.h
$(MSWINC)/wx_mtxt.h:  $(BASEINC)/wb_mtxt.h
$(MSWINC)/wx_slidr.h:  $(BASEINC)/wb_slidr.h
$(MSWINC)/wx_menu.h:  $(BASEINC)/wb_menu.h


clean:
	-del *.obj

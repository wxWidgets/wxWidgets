#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 9 November 1999                                                     *
#                                                                            *
#*****************************************************************************
.first
	set def [-]
	wx_curdir = f$environment("default")
	wx_sub = f$element(0,"]",wx_curdir)
	wx_fuldir = "''wx_sub'.]"
	define/job/trans=(concealed) wx_root "''wx_fuldir'"
	set def [.wxwindows]

all :
	make gtk
	purge [...]
	delete [...]*.obj;
	make motif
	purge [...]
	delete [...]*.obj;

gtk : setup.h
	set default [.src.generic]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.common]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.unix]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.gtk]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [--.samples.minimal]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)

motif : setup.h
	set default [.src.generic]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.common]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.unix]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.motif]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [--.demos.bombs]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [--.samples.caret]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.checklst]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.config]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.controls]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.dialogs]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.image]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.menu]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.minimal]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [--.utils.dialoged.src]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [--]

setup.h : setup.h_vms
	copy setup.h_vms setup.h

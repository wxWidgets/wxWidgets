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
	set def [.wxwidgets]

all :
	set default [.include.wx]
	if f$search("DEPRECATED.DIR") .eqs. "" then set file/enter=[]deprecated.dir [--.contrib.include.wx]deprecated.dir
	set default [--]
	make gtk
	purge [...]
	delete [...]*.obj;
	make motif
	purge [...]
	delete [...]*.obj;
	make x11
	purge [...]
	delete [...]*.obj;

gtk : [.include.wx]setup.h
	set default [.src.generic]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.common]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.html]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.unix]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.gtk]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [--.contrib.src.deprecated]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [---.demos.bombs]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [--.samples.calendar]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.caret]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.checklst]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.config]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.controls]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.db]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.dialogs]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.dialup]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.dnd]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.docview]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.drawing]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.font]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.image]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.mdi]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.menu]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.minimal]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.richedit]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [--]

x11 : [.include.wx]setup.h
	set default [.src.generic]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.common]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.html]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.unix]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.x11]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.univ]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [--.contrib.src.deprecated]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [---.demos.bombs]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [--.samples.calendar]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.caret]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.checklst]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.config]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.controls]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.db]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.dialogs]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.dialup]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.dnd]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.docview]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.drawing]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.font]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.image]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.mdi]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.menu]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.minimal]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.richedit]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [--]

motif : [.include.wx]setup.h
	set default [.src.generic]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.common]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.unix]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.motif]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.x11]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [--.contrib.src.deprecated]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [---.demos.bombs]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [--.samples.calendar]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.caret]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.checklst]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.config]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.dialogs]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.image]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.mdi]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.menu]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.minimal]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [--]

[.include.wx]setup.h : setup.h_vms
	copy setup.h_vms [.include.wx]setup.h

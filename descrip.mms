#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 27 August 2013                                                      *
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
	if f$search("GNOME$ROOT:[000000]GLIB2.COM") .nes. "" then make gtk2
	if f$search("GNOME$ROOT:[000000]GLIB2.COM") .nes. "" then purge [...]
	if f$search("GNOME$ROOT:[000000]GLIB2.COM") .nes. "" then delete [...]*.obj;

gtk : [.include.wx]setup.h
	set default [.src.generic]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
	set default [-.common]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
	set default [-.html]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
	set default [-.xml]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.aui]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
	set default [-.xrc]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
	set default [-.unix]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
	set default [-.propgrid]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
	set default [-.regex]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.ribbon]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
	set default [-.richtext]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
	set default [-.stc]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.gtk1]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk.olb [.CXX_REPOSITORY]*.obj
	set default [--.demos.bombs]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.forty]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.fractal]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.life]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.poem]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [--.samples.calendar]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.access]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.animate]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.artprov]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.aui]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.caret]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.clipboard]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.collpane]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.combo]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.config]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.console]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.controls]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.dataview]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.debugrpt]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.dialogs]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.dialup]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.dnd]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.docview]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.dragimag]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.drawing]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.erase]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.event]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.exec]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.font]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.image]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.keyboard]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.mdi]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.mediaplayer]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.menu]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.minimal]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.notebook]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.propgrid]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.regtest]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.ribbon]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.richtext]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.sockets]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.splash]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.stc]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.thread]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.xrc]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [-.widgets]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)
	set default [--.tests]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK__=1)/ignore=warning
	set default [-]

gtk2 : [.include.wx]setup.h
	@gnome$root:[000000]glib2
	set default [.src.generic]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
	set default [-.common]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
	set default [-.html]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
	set default [-.aui]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
	set default [-.xml]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.xrc]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
	set default [-.unix]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
	set default [-.propgrid]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
	set default [-.regex]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.ribbon]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
	set default [-.richtext]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
	set default [-.stc]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.gtk]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_gtk2.olb [.CXX_REPOSITORY]*.obj
	set default [--.demos.bombs]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [--.samples.calendar]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.caret]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.config]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.controls]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.dialogs]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.dialup]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.dnd]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.docview]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.drawing]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.font]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.image]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.mdi]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.menu]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.minimal]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.richedit]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [-.thread]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXGTK2__=1)
	set default [--]
	@gnome$root:[000000]glib

x11 : [.include.wx]setup.h
	set default [.src.generic]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
	set default [-.common]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
	set default [-.html]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
	set default [-.xml]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.xrc]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
	set default [-.aui]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
	set default [-.unix]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
	set default [-.propgrid]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
	set default [-.regex]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.ribbon]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
	set default [-.richtext]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
	set default [-.stc]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.x11]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
	set default [-.univ]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_x11_univ.olb [.CXX_REPOSITORY]*.obj
	set default [--.demos.bombs]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [--.samples.calendar]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.caret]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.config]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.controls]
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
	set default [-.notebook]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.richedit]
#	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.thread]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [-.stc]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXX11__=1,__WXUNIVERSAL__=1)
	set default [--]

motif : [.include.wx]setup.h
	set default [.src.generic]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_motif.olb [.CXX_REPOSITORY]*.obj
	set default [-.common]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_motif.olb [.CXX_REPOSITORY]*.obj
	set default [-.unix]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_motif.olb [.CXX_REPOSITORY]*.obj
	set default [-.html]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_motif.olb [.CXX_REPOSITORY]*.obj
	set default [-.aui]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_motif.olb [.CXX_REPOSITORY]*.obj
	set default [-.regex]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.motif]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	If f$getsyi("HW_MODEL") .le. 2048 then library [--.lib]libwx_motif.olb [.CXX_REPOSITORY]*.obj
	set default [-.x11]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [--.demos.bombs]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [--.samples.calendar]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.caret]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.aui]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.combo]
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
	set default [-.notebook]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.thread]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [-.widgets]
	$(MMS)$(MMSQUALIFIERS)/macro=(__WXMOTIF__=1)
	set default [--]

[.include.wx]setup.h : setup.h_vms
	copy setup.h_vms [.include.wx]setup.h

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

all : setup.h
	set default [.src.generic]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.common]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.unix]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.motif]
	$(MMS)$(MMSQUALIFIERS)
	set default [--.samples.bombs]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.caret]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.checklst]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.config]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.controls]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.dialogs]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.image]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.menu]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.minimal]
	$(MMS)$(MMSQUALIFIERS)
	set default [--.utils.dialoged.src]
	$(MMS)$(MMSQUALIFIERS)

setup.h : setup.h_vms
	copy setup.h_vms setup.h

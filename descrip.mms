#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 9 November 1999                                                     *
#                                                                            *
#*****************************************************************************

all : setup.h
	set default [.src.generic]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.common]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.unix]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.motif]
	$(MMS)$(MMSQUALIFIERS)
	set default [--.samples.menu]
	$(MMS)$(MMSQUALIFIERS)

setup.h : setup.h_vms
	copy setup.h_vms setup.h

#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 10 November 1999                                                     *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)
.else
CXX_DEFINE =
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp

all :
	$(MMS)$(MMSQUALIFIERS) controls.exe

controls.exe : controls.obj
.ifdef __WXMOTIF__
	cxxlink controls,[--.lib]vms/opt
.endif

controls.obj : controls.cpp

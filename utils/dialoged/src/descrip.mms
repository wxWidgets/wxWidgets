#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 10 November 1999                                                     *
#                                                                            *
#*****************************************************************************
.first
	define wx [---.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)
.else
CXX_DEFINE =
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp

all :
	$(MMS)$(MMSQUALIFIERS) dialoged.exe

OBJECTS=dialoged.obj,reseditr.obj,dlghndlr.obj,reswrite.obj,winprop.obj,\
	edtree.obj,edlist.obj,symbtabl.obj,winstyle.obj

dialoged.exe : $(OBJECTS)
.ifdef __WXMOTIF__
	cxxlink $(OBJECTS),[---.lib]vms/opt
.endif

dialoged.obj : dialoged.cpp
reseditr.obj : reseditr.cpp
dlghndlr.obj : dlghndlr.cpp
reswrite.obj : reswrite.cpp
winprop.obj : winprop.cpp
edtree.obj : edtree.cpp
edlist.obj : edlist.cpp
symbtabl.obj : symbtabl.cpp
winstyle.obj : winstyle.cpp

#
# Changelist: 2003-02-25 - Juergen Ulbts - update from wxWindows 2.5.x/HEAD branch
#
# $+ .. $- means we get the result directly
THISDIR = $+ $(%cdrive):$(%cwd) $-

WXDIR = $(%WXWIN)
!ifndef EXEDIR
OUTPUTDIR = $(THISDIR)\Watcom 
!else
OUTPUTDIR = $(THISDIR)\$(EXEDIR)
!ifeq EXEDIR "."
OUTPUTDIR = $(THISDIR)
!endif
!endif

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib
LNK = $(OUTPUTDIR)\$(PROGRAM).lnk

all: MAKEDIR $(OUTPUTDIR)\$(PROGRAM).exe $(EXTRATARGETS) .SYMBOLIC

MAKEDIR: .SYMBOLIC
	@if not exist $(OUTPUTDIR) mkdir $(OUTPUTDIR)

!ifeq wxUSE_GUI 0
RESFILE=
!else
RESFILE=$(OUTPUTDIR)\$(PROGRAM).res
!endif

$(OUTPUTDIR)\$(PROGRAM).exe : $(OBJECTS) $(RESFILE) $(LNK) $(WXLIB)\$(LIBNAME).lib
    wlink @$(LNK)
!ifneq wxUSE_GUI 0
    $(BINDCOMMAND) $(RESFILE)
!endif

!ifneq wxUSE_GUI 0
$(RESFILE): $(PROGRAM).rc $(WXDIR)\include\wx\msw\wx.rc
     $(RC) $(RESFLAGS) /fo=$(RESFILE) $(PROGRAM).rc
!endif

$(LNK) : .SYMBOLIC
    %create $(LNK)
    @%append $(LNK) $(LDFLAGS)
    @%append $(LNK) $(DEBUGINFO)
    @%append $(LNK) system $(LINKSYSTEM)
    @%append $(LNK) $(STACK)
    @%append $(LNK) name $(OUTPUTDIR)\$(PROGRAM).exe
    @for %i in ($(OBJECTS)) do @%append $(LNK) file %i
    @for %i in ($(LIBS)) do @%append $(LNK) lib %i
    @for %i in ($(EXTRALIBS)) do @%append $(LNK) lib %i

clean:   .SYMBOLIC
    -erase $(OUTPUTDIR)\*.obj
    -erase $(OUTPUTDIR)\*.bak
    -erase $(OUTPUTDIR)\*.err
    -erase $(OUTPUTDIR)\*.pch
    -erase $(OUTPUTDIR)\*.lib
    -erase $(LNK)
    -erase $(OUTPUTDIR)\*.res
    -erase $(OUTPUTDIR)\*.exe
    -erase $(OUTPUTDIR)\*.lbc
	-rmdir $(OUTPUTDIR)


#
# Changelist:	2003-02-25 - Juergen Ulbts - update from wxWindows 2.5.x/HEAD branch
#

OUTPUTDIR= $+ $(%cdrive):$(%cwd)\WATCOM $-

!include $(WXDIR)\src\makewat.env


all: MAKDIR $(LIBTARGET) .SYMBOLIC

MAKDIR:  .SYMBOLIC
	@if not exist $(OUTPUTDIR) mkdir $(OUTPUTDIR)

# the name of the file containing the objects to be put in the library
LBCFILE=$(LIBTARGET).lbc
$(LIBTARGET) : $(OBJECTS)
    %create $(LBCFILE)
    @for %i in ( $(OBJECTS) ) do @%append $(LBCFILE) +%i
    $(LIB) /b /c /n /p=512 $^@ @$(LBCFILE)

clean:   .SYMBOLIC
    -erase $(OUTPUTDIR)\*.obj
    -erase $(OUTPUTDIR)\*.bak
    -erase $(OUTPUTDIR)\*.err
    -erase $(OUTPUTDIR)\*.pch
    -erase $(OUTPUTDIR)\*.lib
    -erase $(OUTPUTDIR)\*.lbc
    -erase $(OUTPUTDIR)\*.res
    -erase $(OUTPUTDIR)\*.exe
    -erase $(OUTPUTDIR)\*.lbc
    -erase $(LIBTARGET)


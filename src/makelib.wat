#
# Changelist:	2003-02-25 - Juergen Ulbts - update from wxWindows 2.5.x/HEAD branch
#

<<<<<<< makelib.wat
THISDIR = $+ $(%cdrive):$(%cwd) $-
# set OUTPUTDIR to the directory to be used for the .obj files created during
# the build (under $THISDIR)
!ifndef OUTPUTDIR
OUTPUTDIR=$(THISDIR)\$(BASEDIRPREFIX)$(CONFIGURATION)Watcom
# no trailing slash for win98
!endif

=======
OUTPUTDIR= $+ $(%cdrive):$(%cwd)\WATCOM $-

>>>>>>> 1.1.8.2
!include $(WXDIR)\src\makewat.env
<<<<<<< makelib.wat

all: $(OUTPUTDIR) $(LIBTARGET) .SYMBOLIC
=======

>>>>>>> 1.1.8.2

<<<<<<< makelib.wat
$(OUTPUTDIR):
	@if not exist $^@ mkdir $^@
=======
all: MAKDIR $(LIBTARGET) .SYMBOLIC
>>>>>>> 1.1.8.2

<<<<<<< makelib.wat
=======
MAKDIR:  .SYMBOLIC
	@if not exist $(OUTPUTDIR) mkdir $(OUTPUTDIR)

>>>>>>> 1.1.8.2
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


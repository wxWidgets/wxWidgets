!include $(WXDIR)\src\makewat.env

all: $(OUTPUTDIR) $(LIBTARGET)

$(OUTPUTDIR):
	@if not exist $^@ mkdir $^@

# the name of the file containing the objects to be put in the library
LBCFILE=$(OUTPUTDIR)$(LIBTARGET).lbc
$(LIBTARGET) : $(OBJECTS)
    %create $(LBCFILE)
    @for %i in ( $(OBJECTS) ) do @%append $(LBCFILE) +%i
    $(LIB) /b /c /n /p=512 $^@ @$(LBCFILE)

clean:   .SYMBOLIC
    -erase $(OUTPUTDIR)*.obj
    -erase $(OUTPUTDIR)*.bak
    -erase $(OUTPUTDIR)*.err
    -erase $(OUTPUTDIR)*.pch
    -erase $(OUTPUTDIR)*.lib
    -erase $(OUTPUTDIR)*.lbc
    -erase $(OUTPUTDIR)*.res
    -erase $(OUTPUTDIR)*.exe
    -erase $(OUTPUTDIR)*.lbc
    -erase $(LIBTARGET)


WXDIR = $(%WXWIN)

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib
LNK = tmp.lbc

all: $(LIBTARGET)

$(LIBTARGET) : $(OBJECTS)
    %create tmp.lbc
    @for %i in ( $(OBJECTS) ) do @%append tmp.lbc +%i
    wlib /b /c /n /p=512 $^@ @tmp.lbc

clean:   .SYMBOLIC
    -erase *.obj
    -erase *.bak
    -erase *.err
    -erase *.pch
    -erase *.lib
    -erase *.lbc
    -erase *.res
    -erase *.exe
    -erase *.lbc
    -erase $(LIBTARGET)


!include makewat.env

# the name of the file containing the objects to be put in the library
LNK = tmp.lbc

all: $(LIBTARGET)

$(LIBTARGET) : $(OBJECTS)
    %create $(LNK)
    @for %i in ( $(OBJECTS) ) do @%append $(LNK) +%i
    wlib /b /c /n /p=512 $^@ @$(LNK)

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


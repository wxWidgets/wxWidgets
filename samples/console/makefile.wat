wxUSE_GUI = 0
OUTPUTDIR = Watcom\

PROGRAM = console
OBJECTS = $(OUTPUTDIR)$(PROGRAM).obj

!include $(%WXWIN)\src\makeprog.wat



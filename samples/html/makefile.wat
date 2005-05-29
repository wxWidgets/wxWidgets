#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1993
# Updated:	
# Copyright:	(c) 1993, AIAI, University of Edinburgh
#
# "%W% %G%"
#
# Makefile : Builds samples for windows with Watcom
# Use FINAL=1 argument to wmake to build final version with no debugging
# info

# Set WXDIR for your system
WXDIR = $(%WXWIN)

THISDIR=$(WXDIR)\samples\html

!include $(WXDIR)\src\makewat.env

all:
        cd $(WXDIR)\samples\html\about
        wmake -f makefile.wat FINAL=$(FINAL) WXUSINGDLL=$(WXUSINGDLL)
        cd $(WXDIR)\samples\html\help
        wmake -f makefile.wat FINAL=$(FINAL) WXUSINGDLL=$(WXUSINGDLL)
        cd $(WXDIR)\samples\html\helpview
        wmake -f makefile.wat FINAL=$(FINAL) WXUSINGDLL=$(WXUSINGDLL)
        cd $(WXDIR)\samples\html\printing
        wmake -f makefile.wat FINAL=$(FINAL) WXUSINGDLL=$(WXUSINGDLL)
        cd $(WXDIR)\samples\html\test
        wmake -f makefile.wat FINAL=$(FINAL) WXUSINGDLL=$(WXUSINGDLL)
        cd $(WXDIR)\samples\html\virtual
        wmake -f makefile.wat FINAL=$(FINAL) WXUSINGDLL=$(WXUSINGDLL)
        cd $(WXDIR)\samples\html\widget
        wmake -f makefile.wat FINAL=$(FINAL) WXUSINGDLL=$(WXUSINGDLL)
        cd $(WXDIR)\samples\html\zip
        wmake -f makefile.wat FINAL=$(FINAL) WXUSINGDLL=$(WXUSINGDLL)

clean:
        cd $(WXDIR)\samples\html\about
        wmake -f makefile.wat clean
        cd $(WXDIR)\samples\html\help
        wmake -f makefile.wat clean
        cd $(WXDIR)\samples\html\helpview
        wmake -f makefile.wat clean
        cd $(WXDIR)\samples\html\printing
        wmake -f makefile.wat clean
        cd $(WXDIR)\samples\html\test
        wmake -f makefile.wat clean
        cd $(WXDIR)\samples\html\virtual
        wmake -f makefile.wat clean
        cd $(WXDIR)\samples\html\widget
        wmake -f makefile.wat clean
        cd $(WXDIR)\samples\html\zip
        wmake -f makefile.wat clean


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
WXDIR=$(%WXWIN)

THISDIR=$(WXDIR)\samples

!include $(WXDIR)\src\makewat.env

MAKEFLAGS=FINAL=$(FINAL) UNICODE=$(UNICODE) WXUSINGDLL=$(WXUSINGDLL)

#
# Please keep samples list up-to-date, in alphabetical order
#
all:
        cd $(WXDIR)\samples\artprov
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\calendar
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\caret
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\checklst
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\config
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\controls
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\db
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\dialogs
        -wmake -f makefile.wat $(MAKEFLAGS)
!if "$(COMPIL)"==""
        cd $(WXDIR)\samples\dialup
        -wmake -f makefile.wat $(MAKEFLAGS)
!endif
        cd $(WXDIR)\samples\dnd
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\docview
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\docvwmdi
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\dragimag
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\drawing
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\dynamic
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\erase
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\event
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\font
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\grid
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\help
        -wmake -f makefile.wat $(MAKEFLAGS)
!if "$(PNGSETUP)"==""
        cd $(WXDIR)\samples\html
        -wmake -f makefile.wat $(MAKEFLAGS)
!endif
        cd $(WXDIR)\samples\image
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\internat
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\ipc
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\joytest
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\keyboard
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\layout
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\listctrl
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\mdi
        -wmake -f makefile.wat $(MAKEFLAGS)
#!if "$(FINAL)" == "0"
#        cd $(WXDIR)\samples\memcheck
#        -wmake -f makefile.wat $(MAKEFLAGS)
#!endif
        cd $(WXDIR)\samples\menu
        -wmake -f makefile.wat $(MAKEFLAGS)
!if "$(COMPIL)"==""
        cd $(WXDIR)\samples\mfc
        -wmake -f makefile.wat $(MAKEFLAGS)
!endif
        cd $(WXDIR)\samples\minifram
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\minimal
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\nativdlg
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\newgrid
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\notebook
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\oleauto
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\ownerdrw
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\png
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\printing
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\proplist
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\propsize
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\regtest
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\resource
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\rotate
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\sashtest
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\scroll
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\scrollsub
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\sockets
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\splitter
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\statbar
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\taskbar
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\text
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\thread
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\toolbar
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\treectrl
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\typetest
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\validate
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\widgets
        -wmake -f makefile.wat $(MAKEFLAGS)
        cd $(WXDIR)\samples\wizard
        -wmake -f makefile.wat $(MAKEFLAGS)

clean:
        cd $(WXDIR)\samples\calendar
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\caret
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\checklst
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\config
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\controls
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\db
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\dialogs
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\dialup
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\dnd
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\docview
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\docvwmdi
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\drawing
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\dragimag
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\dynamic
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\erase
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\event
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\font
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\grid
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\help
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\html
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\image
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\internat
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\ipc
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\joytest
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\keyboard
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\layout
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\listctrl
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\mdi
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\mfc
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\memcheck
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\menu
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\minifram
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\minimal
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\nativdlg
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\newgrid
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\notebook
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\oleauto
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\ownerdrw
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\png
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\printing
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\proplist
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\propsize
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\regtest
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\resource
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\rotate
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\sashtest
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\scroll
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\scrollsub
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\sockets
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\splitter
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\statbar
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\taskbar
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\text
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\thread
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\toolbar
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\treectrl
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\typetest
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\validate
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\widgets
        -wmake -f makefile.wat clean
        cd $(WXDIR)\samples\wizard
        -wmake -f makefile.wat clean

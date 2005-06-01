# 
# Makefile for WATCOM 
# 
# Created by Julian Smart, January 1999 
#  
# 
# 
# 
PROGRAM = docview 
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj $(OUTPUTDIR)\doc.obj $(OUTPUTDIR)\view.obj 
# 
!include $(%WXWIN)\src\makeprog.wat 

# 
# Makefile for WATCOM 
# 
# Created by Julian Smart, January 1999 
#  
# 
# 
THISDIR = $(%WXWIN)\samples\newgrid 
# 
PROGRAM = newgrid 
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj 
# 
!include $(%WXWIN)\src\makeprog.wat 

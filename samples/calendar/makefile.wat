# 
# Makefile for WATCOM 
# 
# Created by Julian Smart, January 1999 
#  
# 
# 
THISDIR = $(%WXWIN)\samples\calendar 
# 
PROGRAM = calendar 
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj 
# 
!include $(%WXWIN)\src\makeprog.wat 

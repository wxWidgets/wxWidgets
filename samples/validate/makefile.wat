# 
# Makefile for WATCOM 
# 
# Created by Julian Smart, January 1999 
#  
# 
# 
THISDIR = $(%WXWIN)\samples\validate 
# 
PROGRAM = validate 
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj 
# 
!include $(%WXWIN)\src\makeprog.wat 

# 
# Makefile for WATCOM 
# 
# Created by Julian Smart, January 1999 
#  
# 
# 
THISDIR = $(%WXWIN)\samples\minifram 
# 
PROGRAM = minifram 
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj 
# 
!include $(%WXWIN)\src\makeprog.wat 

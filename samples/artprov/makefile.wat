# 
# Makefile for WATCOM 
# 
# Created by Julian Smart, January 1999 
#  
# 
# 
THISDIR = $(%WXWIN)\samples\artprov 
# 
PROGRAM = artprov 
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj 
# 
!include $(%WXWIN)\src\makeprog.wat 

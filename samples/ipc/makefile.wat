# 
# Makefile for WATCOM 
# 
# Created by Julian Smart, January 1999 
#  
# 
# 
# 
!include $(%WXWIN)\src\makewat.env

all:
    wmake -f server.wat FINAL=$(FINAL) WXUSINGDLL=$(WXUSINGDLL)
    wmake -f client.wat FINAL=$(FINAL) WXUSINGDLL=$(WXUSINGDLL)

clean:
    wmake -f server.wat clean
    wmake -f client.wat clean
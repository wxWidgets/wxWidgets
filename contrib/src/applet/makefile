#############################################################################
#
#                   Copyright (C) 1999 SciTech Software
#                           All rights reserved.
#
# Description:   Generic makefile for the SciTech wxApplet library
# License:       wxWindows license
#
#############################################################################

# We are building with the Win32 version

USE_RTTI		:= 1
USE_CPPEXCEPT	:= 1
PRECOMP_HDR		:= wx/wxprec.h
CFLAGS          += -D__WIN95__ -D__WXMSW__ -D__WINDOWS__

# Link with wxWindows static link libraries or with the DLL. DLL is default.

.IF $(STATIC_LINK)
.ELSE
CFLAGS          += -DWXUSINGDLL
.ENDIF

# Define the library name and objects

LIBFILE         = wxapplet$L
LIBCLEAN        = *.il? *.dll *.lib
OBJECTS			:= applet$O appletwindow$O

DEPEND_OBJ      = $(OBJECTS)
.INCLUDE: "$(SCITECH)\makedefs\common.mk"


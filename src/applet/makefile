#############################################################################
#
#                   Copyright (C) 1999 SciTech Software
#                           All rights reserved.
#
# Descripton:   Generic makefile for the SciTech wxApplet library
#
#############################################################################

# We are building with the Win32 version

USE_RTTI		:= 1
USE_CPPEXCEPT	:= 1
PRECOMP_HDR		:= wx/wxprec.h
CFLAGS          += -D__WIN95__ -D__WXMSW__ -D__WINDOWS__

# Define the library name and objects

LIBFILE         = wxapplet$L
LIBCLEAN        = *.il? *.dll *.lib
OBJECTS			:= applet$O appletwindow$O

# Define the sample program to be compiled

EXELIBS         = wxwin$L png$L jpeg$L tiff$L xpm$L ddstereo$L comctl32$L uuid$L
SAMP_OBJECTS	= sample$O sample$R monitorapplet$O

sample$E: 		$(SAMP_OBJECTS) $(LIBFILE)

DEPEND_OBJ      = $(OBJECTS) $(SAMP_OBJECTS)
DEPEND_SRC      = sample
.SOURCE:		  sample
.INCLUDE: "$(SCITECH)\makedefs\common.mk"


#
# File:		Makefile
# Author:	Robert Roebling
# Created:	1999
# Updated:	
# Copyright:	(c) 1998 Robert Roebling
#
# Makefile for OpenGl demo (GTK version)
#
# This makefile requires wxWindows/GTK to be 
# installed (possibly using "make install")
# on your system.
#

CC = g++
WXCONFIG=../../../../wx-config
WXINCLUDE=-I../../../../include
WXLIB=-L../../../../lib

isosurf: isosurf.o glcanvas.o
	$(CC) -o isosurf \
	isosurf.o glcanvas.o \
	`$(WXCONFIG) --libs` $(WXLIB) -lMesaGL -lMesaGLU

isosurf.o: isosurf.cpp
	$(CC) `$(WXCONFIG) --cflags` -I../../gtk $(WXINCLUDE) -c isosurf.cpp

glcanvas.o: ../../gtk/glcanvas.cpp
	$(CC) `$(WXCONFIG) --cflags` `gtk-config --cflags` -I../../gtk -c $(WXINCLUDE) ../../gtk/glcanvas.cpp

clean: 
	rm -f *.o isosurf

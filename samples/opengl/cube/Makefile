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

CC = gcc
WXCONFIG=../../../wx-config
WXINCLUDE=-I../../../include
WXLIB=-L../../../lib

cube: cube.o # glcanvas.o
	$(CC) -o cube cube.o `$(WXCONFIG) --libs` $(WXLIB) -lMesaGL -lMesaGLU

cube.o: cube.cpp
	$(CC) `$(WXCONFIG) --cflags` -I../../gtk $(WXINCLUDE) -c cube.cpp

glcanvas.o: ../../gtk/glcanvas.cpp
	$(CC) `$(WXCONFIG) --cflags` `gtk-config --cflags` -I../../gtk $(WXINCLUDE) -c ../../gtk/glcanvas.cpp

clean: 
	rm -f *.o cube


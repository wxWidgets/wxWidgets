#
# File:		Makefile
# Author:	Robert Roebling
# Created:	1999
# Updated:	
# Copyright:	(c) 1998 Robert Roebling
#
# Makefile for OpenGL demo (GTK version)
#
# This makefile requires wxWindows/GTK to be 
# installed (possibly using "make install")
# on your system.
#

CPP = gcc
CC = gcc
WXCONFIG=../../../wx-config
WXINCLUDE=-I../../../include
WXLIB=-L../../../lib

Penguin: penguin.o trackball.o lw.o # glcanvas.o
	$(CPP) -o Penguin penguin.o trackball.o lw.o `$(WXCONFIG) --libs` $(WXLIB) -lMesaGL -lMesaGLU

penguin.o: penguin.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../gtk $(WXINCLUDE) -c penguin.cpp

lw.o: lw.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../gtk $(WXINCLUDE) -c lw.cpp

trackball.o: trackball.c
	$(CC) `$(WXCONFIG) --cflags` -I../../gtk $(WXINCLUDE) -c trackball.c

glcanvas.o: ../../gtk/glcanvas.cpp
	$(CPP) `$(WXCONFIG) --cflags` `gtk-config --cflags` -g -I../../gtk $(WXINCLUDE) -c ../../gtk/glcanvas.cpp

clean: 
	rm -f *.o Penguin

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

cube: cube.o glcanvas.o
	$(CC) -o cube cube.o glcanvas.o `wx-config --libs` -lMesaGL -lMesaGLU

cube.o: cube.cpp
	$(CC) `wx-config --cflags` -I../../gtk -c cube.cpp

glcanvas.o: ../../gtk/glcanvas.cpp
	$(CC) `wx-config --cflags` `gtk-config --cflags` -I../../gtk -c ../../gtk/glcanvas.cpp

clean: 
	rm -f *.o cube

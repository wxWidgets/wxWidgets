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

isosurf: isosurf.o glcanvas.o
	$(CC) -o isosurf \
	isosurf.o glcanvas.o \
	`wx-config --libs` -lMesaGL -lMesaGLU

isosurf.o: isosurf.cpp
	$(CC) `wx-config --cflags` -I../../gtk -c isosurf.cpp

glcanvas.o: ../..gtk/glcanvas.cpp
	$(CC) `wx-config --cflags` `gtk-config --cflags` -I../../gtk -c ../../gtk/glcanvas.cpp

clean: 
	rm -f *.o cube

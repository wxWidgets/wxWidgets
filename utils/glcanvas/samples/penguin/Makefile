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

CPP = g++
CC = gcc

Penguin: penguin.o trackball.o lw.o glcanvas.o
	$(CPP) -o Penguin \
	penguin.o trackball.o lw.o glcanvas.o \
	`wx-config --libs` -lMesaGL -lMesaGLU

penguin.o: penguin.cpp
	$(CPP) `wx-config --cflags` -g -I../../gtk -c penguin.cpp

lw.o: lw.cpp
	$(CPP) `wx-config --cflags` -I../../gtk -c lw.cpp

trackball.o: trackball.c
	$(CC) `wx-config --cflags` -I../../gtk -c trackball.c

glcanvas.o: ../../gtk/glcanvas.cpp
	$(CPP) `wx-config --cflags` `gtk-config --cflags` -g -I../../gtk -c ../../gtk/glcanvas.cpp

clean: 
	rm -f *.o Penguin

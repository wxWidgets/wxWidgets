#
# File:		Makefile
# Author:	Robert Roebling
# Created:	1999
# Updated:	
# Copyright:	(c) 1998 Robert Roebling
#
# Makefile for OLE demo (GTK version)
#
# This makefile requires wxWindows/GTK to be 
# installed (possibly using "make install")
# on your system.
#

CC = g++

servlet: servlet.o wxole.o
	$(CC) -o servlet servlet.o wxole.o \
	`wx-config --libs` -lbonobo `gnome-config --libs gnorba` `gnome-config --libs gnomeui` 

servlet.o: servlet.cpp
	$(CC) `wx-config --cflags` `gtk-config --cflags` -I../../gtk -c servlet.cpp

wxole.o: ../../gtk/wxole.cpp
	$(CC) `wx-config --cflags` `gtk-config --cflags` `gnome-config --cflags gnorba` \
	-I../../gtk -c ../../gtk/wxole.cpp

clean: 
	rm -f *.o servlet

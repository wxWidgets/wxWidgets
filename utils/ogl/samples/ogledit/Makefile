#
# File:		Makefile
# Author:	Julian Smart
# Created:	1999
# Updated:	
# Copyright:	(c) 2000 Julian Smart
#
# Makefile for OGL demo (GTK version)
#
# This makefile requires wxWindows/GTK to be 
# installed (possibly using "make install")
# on your system.
#

CPP = gcc
CC = gcc
WXCONFIG=../../../../wx-config
WXINCLUDE=-I../../../../include
WXLIB=-L../../../../lib

OBJECTS=ogledit.o palette.o doc.o view.o

ogledit: $(OBJECTS)
	$(CPP) -o ogledit $(OBJECTS) `$(WXCONFIG) --libs` $(WXLIB) -logl

ogledit.o: ogledit.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c ogledit.cpp

palette.o: palette.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c palette.cpp

doc.o: doc.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c doc.cpp

view.o: view.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c view.cpp

clean: 
	rm -f *.o ogledit

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
WXINCLUDE=-I../../../../include -I../../../include
WXLIB=-L../../../../lib -L../../../lib

OBJECTS=studio.o cspalette.o csprint.o dialogs.o doc.o mainfrm.o project.o shapes.o symbols.o view.o

studio: $(OBJECTS)
	$(CPP) -o studio $(OBJECTS) `$(WXCONFIG) --libs` $(WXLIB) -logl

studio.o: studio.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c studio.cpp

cspalette.o: cspalette.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c cspalette.cpp

doc.o: doc.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c doc.cpp

view.o: view.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c view.cpp

dialogs.o: dialogs.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c dialogs.cpp

mainfrm.o: mainfrm.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c mainfrm.cpp

project.o: project.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c project.cpp

shapes.o: shapes.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c shapes.cpp

symbols.o: symbols.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c symbols.cpp

csprint.o: csprint.cpp
	$(CPP) `$(WXCONFIG) --cflags` -I../../src $(WXINCLUDE) -c csprint.cpp

clean: 
	rm -f *.o studio

#
# File:		Makefile for samples
# Author:	Robert Roebling
# Created:	1999
# Updated:	
# Copyright:	(c) 1998 Robert Roebling
#
# This makefile requires a Unix version of wxWindows
# to be installed on your system. This is most often
# done typing "make install" when using the complete
# sources of wxWindows or by installing the two
# RPM packages wxGTK.XXX.rpm and wxGTK-devel.XXX.rpm
# under Linux.
#

CC = gcc

PROGRAM = bombs

$(PROGRAM): $(PROGRAM).o game.o bombs1.o
	$(CC) -o $(PROGRAM) \
	$(PROGRAM).o bombs1.o game.o `wx-config --libs`

$(PROGRAM).o: $(PROGRAM).cpp
	$(CC) `wx-config --cflags` -c $(PROGRAM).cpp

game.o: game.cpp
	$(CC) `wx-config --cflags` -c game.cpp
	
bombs1.o: bombs1.cpp
	$(CC) `wx-config --cflags` -c bombs1.cpp

clean: 
	rm -f *.o $(PROGRAM)

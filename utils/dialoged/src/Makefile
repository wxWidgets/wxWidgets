#
# File:		Makefile
# Author:	Robert Roebling
# Created:	1999
# Updated:	
# Copyright:	(c) 1998 Robert Roebling
#
# Makefile for Dialog Editor (GTK version)
#
# This makefile requires wxWindows/GTK to be 
# installed (possibly using "make install")
# on your system.
#

CC = g++

DialogEd: dialoged.o dlghndlr.o edlist.o edtree.o reseditr.o reswrite.o \
	symbtabl.o winprop.o winstyle.o
	$(CC) -o DialogEd \
	dialoged.o dlghndlr.o edlist.o edtree.o reseditr.o reswrite.o \
	symbtabl.o winprop.o winstyle.o `wx-config --libs`

dialoged.o: dialoged.cpp
	$(CC) `wx-config --cflags` -c dialoged.cpp

dlghndlr.o: dlghndlr.cpp
	$(CC) `wx-config --cflags` -c dlghndlr.cpp

edlist.o: edlist.cpp
	$(CC) `wx-config --cflags` -c edlist.cpp

edtree.o: edtree.cpp
	$(CC) `wx-config --cflags` -c edtree.cpp

reseditr.o: reseditr.cpp
	$(CC) `wx-config --cflags` -c reseditr.cpp

reswrite.o: reswrite.cpp
	$(CC) `wx-config --cflags` -c reswrite.cpp

symbtabl.o: symbtabl.cpp
	$(CC) `wx-config --cflags` -c symbtabl.cpp

winprop.o: winprop.cpp
	$(CC) `wx-config --cflags` -c winprop.cpp

winstyle.o: winstyle.cpp
	$(CC) `wx-config --cflags` -c winstyle.cpp

clean: 
	rm -f *.o DialogEd

# Purpose: makefile for toolbar example (Unix)
# Created: 2000-03-14

CC = gcc

PROGRAM = wxib

OBJECTS = $(PROGRAM).o

# implementation

.SUFFIXES:	.o .cpp

.cpp.o :
	$(CC) -c `wx-config --cflags` -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) `wx-config --libs`

clean:
	rm -f *.o $(PROGRAM)

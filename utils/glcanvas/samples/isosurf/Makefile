
CC = g++

isosurf: isosurf.o glcanvas.o
	$(CC) -o isosurf \
	isosurf.o glcanvas.o \
	`wx-config --libs` -lMesaGL -lMesaGLU

isosurf.o: isosurf.cpp
	$(CC) `wx-config --cflags` -I../../src -c isosurf.cpp

glcanvas.o: ../../src/glcanvas.cpp
	$(CC) `wx-config --cflags` `gtk-config --cflags` -I../../src -c ../../src/glcanvas.cpp

clean: 
	rm -f *.o cube


CC = g++

cube: cube.o glcanvas.o
	$(CC) -o cube \
	cube.o glcanvas.o \
	`wx-config --libs` -lMesaGL -lMesaGLU

cube.o: cube.cpp
	$(CC) `wx-config --cflags` -I../../src -c cube.cpp

glcanvas.o: ../../src/glcanvas.cpp
	$(CC) `wx-config --cflags` `gtk-config --cflags` -I../../src -c ../../src/glcanvas.cpp

clean: 
	rm -f *.o cube


CC = gcc

test: test.c 
	$(CC) `gtk-config --cflags`  test.c -o test `gtk-config --libs`

clean: 
	rm -f *.o test

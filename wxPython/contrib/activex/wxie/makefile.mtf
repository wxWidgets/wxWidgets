CC = gcc

PROGRAM = wxIE

CFLAGS = -I/m/snowball/icicle/gswd/devenv/include -D__WXMOTIF__
LFLAGS = -L/m/snowball/icicle/devenv/lib

# implementation

.SUFFIXES:      .o .cpp

SOURCES:sh= /bin/ls *.cpp

OBJECTS = $(SOURCES:.cpp=.o)

.cpp.o :
	$(CC) -c $(CFLAGS) `wx-config --cflags` -o $@ $<

$(PROGRAM): $(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) $(LFLAGS) `wx-config --libs`

clean:
	rm -f *.o $(PROGRAM)



CXX = c++
CXXFLAGS = `wx-config --cxxflags` -fPIC -I.
LDFLAGS = `wx-config --libs`


all: test_prg test_dll.so

test_prg: dllwidget.o test_prg.o
	$(CXX) $(LDFLAGS) -o $@ dllwidget.o test_prg.o

test_dll.so: test_dll.o
	$(CXX) $(LDFLAGS) -shared -o $@ $<

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o *.so test_prg

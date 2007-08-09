
CXX = c++
CXXFLAGS = `wx-config --cxxflags` -fPIC -I.
LDFLAGS = `wx-config --libs`


all: test_dll.so

test_dll.so: test_dll.o
	$(CXX) $(LDFLAGS) -shared -o $@ $<

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o *.so

CXX_INCLUDES=-I/usr/local/include/eigen3

all: libquickfix.so

test: test.cpp
	g++ $(CXX_INCLUDES) -o test test.cpp beacon.cpp

%.o: %.cpp
	g++ $(CXX_INCLUDES) -fPIC -c $< -o $@

libquickfix.so: beacon.o wrapper.o
	g++ -shared -W1,-soname,libquickfix.so -o libquickfix.so beacon.o wrapper.o

clean:
	rm -f *.o *.so

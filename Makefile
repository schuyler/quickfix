CXX_INCLUDES=-I/usr/local/include/eigen3
CXX_FLAGS=-march=native -Wall -Ofast -DNDEBUG

all: libquickfix.so

test: test.cpp
	g++ $(CXX_INCLUDES) -o test test.cpp beacon.cpp

%.o: %.cpp
	g++ $(CXX_INCLUDES) $(CXX_FLAGS) -fPIC -c $< -o $@

libquickfix.so: beacon.o wrapper.o
	g++ -shared -W1,-soname,libquickfix.so -o libquickfix.so beacon.o wrapper.o

clean:
	rm -f *.o *.so *.pyc test

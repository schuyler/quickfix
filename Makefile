EIGEN_INCLUDE=$(shell pkg-config --cflags eigen3)
CXX_INCLUDES=$(EIGEN_INCLUDE)
CXX_FLAGS=-march=native -Wall -Ofast -DNDEBUG

all: libquickfix.so libquickfix.a

test: test.cpp
	g++ $(CXX_INCLUDES) -o test test.cpp beacon.cpp

%.o: %.cpp
	g++ $(CXX_INCLUDES) $(CXX_FLAGS) -fPIC -c $< -o $@

libquickfix.so: beacon.o wrapper.o
	g++ -shared -W1,-soname,libquickfix.so -o libquickfix.so beacon.o wrapper.o

libquickfix.a: beacon.o
	ar rvs $@ $<

clean:
	rm -f *.o *.so *.a *.pyc test

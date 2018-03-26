EIGEN_INCLUDE=$(shell pkg-config --cflags eigen3)
CXX_INCLUDES=$(EIGEN_INCLUDE) -Iinclude
CXX_FLAGS=-march=native -Wall -Ofast -DNDEBUG

SRC=$(wildcard src/*.cpp)
OBJ=$(SRC:.cpp=.o)

all: build/libquickfix.so build/libquickfix.a

%.o: %.cpp
	g++ $(CXX_INCLUDES) $(CXX_FLAGS) -fPIC -c $< -o $@

build/libquickfix.so: $(OBJ)
	g++ -shared -W1,-soname,libquickfix.so -o $@ $^

build/libquickfix.a: $(OBJ)
	ar rvs $@ $<

test: build/libquickfix.so
	cd test && python test.py

clean:
	rm -f src/*.o build/* test/*.pyc

.PHONY: test clean

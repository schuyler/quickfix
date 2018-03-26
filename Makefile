EIGEN_INCLUDE=$(shell pkg-config --cflags eigen3)
CXX_INCLUDES=$(EIGEN_INCLUDE) -Iinclude
CXX_FLAGS=-march=native -Wall -Ofast -DNDEBUG -MMD

SRC=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%,build/%,$(SRC:.cpp=.o))

all: build/libquickfix.so build/libquickfix.a

build/%.o: src/%.cpp
	g++ $(CXX_INCLUDES) $(CXX_FLAGS) -fPIC -c $< -o $@

build/libquickfix.so: $(OBJ)
	g++ -shared -W1,-soname,libquickfix.so -o $@ $^

build/libquickfix.a: $(OBJ)
	ar rvs $@ $<

test: build/libquickfix.so
	cd test && python test.py

clean:
	rm -f build/* test/*.pyc

-include $(OBJ:%.o=%.d)

.PHONY: test clean

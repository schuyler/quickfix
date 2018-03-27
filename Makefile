EIGEN_INCLUDE=$(shell pkg-config --cflags eigen3)
CXX_INCLUDES=$(EIGEN_INCLUDE) -Iinclude
CXX_FLAGS=-MMD -Wall
CXX_FLAGS+=-DNDEBUG -Ofast
# -march=native works great on MacOS but gets a little weird on Linux?
# CXX_FLAGS += -march=native

SRC=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%,build/%,$(SRC:.cpp=.o))

all: build/libquickfix.so build/libquickfix.a

build/%.o: src/%.cpp
	@mkdir -p build
	g++ $(CXX_INCLUDES) $(CXX_FLAGS) -fPIC -c $< -o $@

build/libquickfix.so: $(OBJ)
	g++ -shared -o $@ $^

build/libquickfix.a: $(OBJ)
	ar rvs $@ $<

test: build/libquickfix.so
	cd test && python test.py

simulate: build/libquickfix.so
	cd test && python simulate.py

clean:
	rm -f build/* test/*.pyc

-include $(OBJ:%.o=%.d)

.PHONY: test clean

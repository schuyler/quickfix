# QuickFix

QuickFix is a C++ library designed to provide multilateration algorithms for
sensor-based indoor positioning.

## Prerequisites

* Eigen 3
* numpy (for testing)
* PyYAML (for testing

On Ubuntu:

```
$ sudo apt-get install libeigen3-dev python-yaml python-numpy
```

On MacOS:

```
$ brew install eigen numpy
$ pip install PyYAML
```

## Building

QuickFix is implemented as a header library, akin to Eigen. For testing purposes,
a wrapper is included that builds a binary, which can be loaded in Python.

```
$ make
```

## Testing

```
$ cd test
$ python test.py
$ python simulate.py
```

You can also mess with `test.yaml` and `simulate.yaml` to adjust the
test/simulation conditions. (The simulation parameter docs are still TBD.)

## Note

This is still a work in progress.

import numpy as np
from ctypes import *

lib = cdll.LoadLibrary("../build/libquickfix.so")

lib.beacon2d_new.argtypes = [c_float] * 4
lib.beacon2d_new.restype = c_void_p

lib.beacon2d_range.argtypes = [c_void_p, c_int, c_float]
lib.beacon2d_range.restype = None

lib.beacon2d_anchor.argtypes = [c_void_p, c_int, c_float, c_float]
lib.beacon2d_anchor.restype = None

lib.beacon2d_reading.argtypes = [c_void_p, c_float, c_float, c_float]
lib.beacon2d_reading.restype = None

lib.beacon2d_fix.argtypes = [c_void_p, c_float, c_float]
lib.beacon2d_fix.restype = None

lib.beacon2d_update.argtypes = [c_void_p, c_float, c_float]
lib.beacon2d_update.restype = c_bool

lib.beacon2d_x.argtypes = [c_void_p]
lib.beacon2d_x.restype = c_float

lib.beacon2d_y.argtypes = [c_void_p]
lib.beacon2d_y.restype = c_float

lib.beacon2d_error.argtypes = [c_void_p]
lib.beacon2d_error.restype = c_float

lib.beacon2d_anchors.argtypes = [c_void_p]
lib.beacon2d_anchors.restype = c_int

class Beacon2D(object):
    def __init__(self, bound):
        # bound is a pair of array-likes, min and max
        bound = map(float, bound[0] + bound[1])
        self.obj = lib.beacon2d_new(*bound)

    def range(self, id_, r):
        lib.beacon2d_range(self.obj, id_, float(r))

    def anchor(self, id_, xy):
        lib.beacon2d_anchor(self.obj, id_, float(xy[0]), float(xy[1]))

    def reading(self, xy, r):
        lib.beacon2d_reading(self.obj, float(xy[0]), float(xy[1]), float(r))

    def fix(self, tick, rms_err):
        lib.beacon2d_fix(self.obj, tick, rms_err)
        return self.position(), self.error()

    def update(self, tick, rms_err):
        lib.beacon2d_update(self.obj, tick, rms_err)

    def position(self):
        x = lib.beacon2d_x(self.obj)
        y = lib.beacon2d_y(self.obj)
        return np.array((x, y))

    def error(self):
        return lib.beacon2d_error(self.obj)

    def anchors(self):
        return lib.beacon2d_anchors(self.obj)

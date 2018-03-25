import numpy as np
from ctypes import *

lib = cdll.LoadLibrary("./libquickfix.so")

lib.Beacon2D_new.argtypes = [c_float] * 4
lib.Beacon2D_new.restype = c_void_p

lib.Beacon2D_Range.argtypes = [c_void_p, c_int, c_float]
lib.Beacon2D_Range.restype = None

lib.Beacon2D_Anchor.argtypes = [c_void_p, c_int, c_float, c_float]
lib.Beacon2D_Anchor.restype = None

lib.Beacon2D_Fix.argtypes = [c_void_p, c_float]
lib.Beacon2D_Fix.restype = None

lib.Beacon2D_X.argtypes = [c_void_p]
lib.Beacon2D_X.restype = c_float

lib.Beacon2D_Y.argtypes = [c_void_p]
lib.Beacon2D_Y.restype = c_float

lib.Beacon2D_Error.argtypes = [c_void_p]
lib.Beacon2D_Error.restype = c_float

class Beacon2D(object):
    def __init__(self, bound):
        # bound is a pair of array-likes, min and max
        bound = map(float, bound[0] + bound[1])
        self.obj = lib.Beacon2D_new(*bound)

    def range(self, id_, r):
        lib.Beacon2D_Range(self.obj, id_, r)

    def anchor(self, id_, xy):
        lib.Beacon2D_Anchor(self.obj, id_, float(xy[0]), float(xy[1]))

    def fix(self, rms_err):
        lib.Beacon2D_Fix(self.obj, rms_err)
        x = lib.Beacon2D_X(self.obj)
        y = lib.Beacon2D_Y(self.obj)
        mse = lib.Beacon2D_Error(self.obj)
        return np.array((x, y)), mse

from ctypes import *

lib = cdll.LoadLibrary("./libquickfix.so")

lib.Beacon2D_new.argtypes = []
lib.Beacon2D_new.restype = c_void_p

lib.Beacon2D_Range.argtypes = [c_int, c_float]
lib.Beacon2D_Range.restype = None

lib.Beacon2D_Anchor.argtypes = [c_int, c_float, c_float]
lib.Beacon2D_Anchor.restype = None

lib.Beacon2D_Fix.argtypes = [c_float]
lib.Beacon2D_Fix.restype = None

lib.Beacon2D_X.argtypes = []
lib.Beacon2D_X.restype = c_float

lib.Beacon2D_Y.argtypes = []
lib.Beacon2D_Y.restype = c_float

lib.Beacon2D_Error.argtypes = []
lib.Beacon2D_Error.restype = c_float

class Beacon2D(object):
    def __init__(self):
        self.obj = lib.Beacon2D_new()

    def range(self, id_, r):
        lib.Beacon2D_Range(self.obj, id_, r)

    def anchor(self, id_, x, y):
        lib.Beacon2D_Anchor(self.obj, id_, x, y)

    def fix(self, rms_err):
        lib.Beacon2D_Fix(self.obj, rms_err)
        x = lib.Beacon2D_X(self.obj)
        y = lib.Beacon2D_Y(self.obj)
        mse = lib.Beacon2D_Error(self.obj)
        return np.array((x, y)), mse

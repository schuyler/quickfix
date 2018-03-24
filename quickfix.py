from ctypes import *

lib = cdll.LoadLibrary("./libquickfix.so")

lib.Beacon2D_new.argtypes = []
lib.Beacon2D_new.restype = c_void_p

lib.Beacon2D_Range.argtypes = [c_int, c_float]
lib.Beacon2D_Range.restype = c_void

lib.Beacon2D_Anchor.argtypes = [c_int, c_float, c_float]
lib.Beacon2D_Anchor.restype = c_void

class Beacon2D(object):
    def __init__(self):
        self.obj = lib.Beacon2D_new()

    def range(self, id_, r):
        lib.Beacon2D_Range(self.obj, id_, r)

    def anchor(self, id_, x, y):
        lib.Beacon2D_Anchor(self.obj, id_, x, y)

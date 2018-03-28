import numpy as np

class KalmanFilter(object):
    def __init__(self, motion_noise, measurement_noise, bounds):
        self.q = motion_noise
        self.r = measurement_noise
        self.speed_limit = 60.
        self.bounds = [
            np.matrix(bounds[0] + [0.0, 0.0]).T,
            np.matrix(bounds[1] + [self.speed_limit, self.speed_limit]).T
        ]
        self.reset()

    def reset(self):
        self.x = None
	self.P = np.matrix(np.identity(4)) * self.q

    def F(self, dt):
        f = np.matrix(np.identity(4))
        f[:2, 2:] = np.identity(2) * dt
        return f

    def G(self, dt):
        return np.matrix((dt**2./2., dt**2./2., dt, dt)).T

    def H(self):
        return np.matrix(np.identity(4))[:2]

    def Q(self, dt, sigma2=None):
        if sigma2 is None: sigma2 = self.q
        G = self.G(dt)
        return G * G.T * sigma2

    def R(self, dt, sigma2=None):
        if sigma2 is None: sigma2 = self.r
        r = np.matrix(np.identity(2)) * sigma2
        return r

    def K(self, dt, r=None):
        P = self.P
        H = self.H()
        R = self.R(dt, r)
        S_k = R + (H * P * H.T)
        K_k = P * H.T * S_k.I
        return K_k

    def predict(self, dt, q=None):
        if self.x is None: return
        if q is None: q = self.q
        F = self.F(dt)
        G = self.G(dt)
        Q = self.Q(dt, q)
        P = self.P
        x = self.x

        #a = np.random.normal(0., q)
        x_k = F * x #+ G * a
        """
        print "\n>>> x:\n", repr(x)
        print "F:\n", repr(F)
        print "G:\n", repr(G)
        print "x_k:\n", repr(x_k)
        """
        P_k = F * P * F.T + Q
        self.x = self.clip(x_k)
        self.P = P_k
        return self.x

    def update(self, dt, z_k, r=None):
        z_k = np.matrix(z_k).T 
        H = self.H()

        if self.x is None:
            self.x = H.T * z_k
            return self.x

        #self.predict(dt)

        if r is None: r = self.r
        P = self.P
        K = self.K(dt, r)
        H = self.H()
        x = self.x

        y_k = z_k - H * x

        """
        print "---\nx:\n", repr(x)
        print "z_k:\n", repr(z_k)
        print "y_k:\n", repr(y_k)
        """
        # if np.linalg.norm(y_k) > self.speed_limit: return

        print "---\nK:", K
        x_k = x + K * y_k
        P_k = P - K * H * P

        """
        print "  x:", x
        print "  y:", y
        print "  K:", K
        print "  P:", P
        """
        self.x = self.clip(x_k)
        self.P = P_k
        return self.x

    def clip(self, x):
        return np.clip(x, self.bounds[0], self.bounds[1])

    def position(self):
        if self.x is None: return None
        return np.asarray(self.x.T[0,:2])

import numpy as np

class ParticleFilter(object):
    def __init__(self, c, d, bounds, n=20, dim=2):
        self.bounds = bounds
        self.dim = dim
        self.c = c
        self.d = d 
        self.n = n
        self.p = None
        self.x = None
        self.reset(0.)

    def reset(self, x):
        # FIXME: this should be a normal distribution around x
        self.p = np.random.uniform(self.bounds[0], self.bounds[1], size=(self.n, self.dim))
        self.w = np.zeros((self.n,))

    def weigh(self, p, x, c):
	dx = np.matrix(np.linalg.norm(p - x, axis=1)).T
	w = np.exp(-dx/(2*c**2))
	return w / sum(w)

    def shuffle(self, p, w, c):
        n = self.n
	i = j = 0
	rounds = 0
	beta = 0
	p2 = np.zeros((self.n, 2))
	while i < self.n:
	    while i < n and np.random.uniform() < w[j]:
		rounds += 1
		p2[i] = p[j]
		i += 1
	    j = (j + 1) % n
	#print rounds
	#print "shuffled:", np.mean(p2, axis=0)
	return p2

    def perturb(self, p, d):
	dp = np.random.normal(0.0, d, size=(self.n,2))
	#print "perturbed:", np.mean(p + dp, axis=0)
	return p + dp

    def update(self, dt, x):
        if self.x is not None and np.abs(self.x - x).any() > self.d * 2:
            #x = self.x + np.clip((x - self.x), self.d*2, self.d*2)
            r = self.d * 2
            c = np.dot(self.x, x) / np.linalg.norm(self.x) / np.linalg.norm(x)
            theta = np.arccos(np.clip(c, -1, 1))
            x = self.x + np.array((r * np.sin(theta), r * np.cos(theta)))
        self.w = self.weigh(self.p, x, dt * self.c)
        p = self.shuffle(self.p, self.w, self.c)
        x_hat = np.mean(p, axis=0)
        self.p = self.perturb(p, self.d)
        self.x = self.clip(x_hat)

    def predict(self, dt):
        pass

    def clip(self, x):
        return np.clip(x, self.bounds[0], self.bounds[1])

    def position(self):
        if self.x is None: return None
        return self.x

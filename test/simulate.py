import numpy as np
import math, time, yaml
from quickfix import Beacon2D

def distance(a, b):
    return np.linalg.norm(a - b)

class Target(object):
    def __init__(self, bounds, speed=60., wander=3., dim=2):
        self.bounds = bounds
        self.position = np.random.uniform(bounds[0], bounds[1], (dim,))
        self.speed = speed
        self.direction = np.random.uniform(0, 2*math.pi)
        self.wander = wander

    def move(self):
        r = np.clip(np.random.normal(self.speed / 2., self.speed / 4.), 0., self.speed)
        z = r * np.exp(1j * self.direction)
        pos = self.position + np.array((np.real(z), np.imag(z)))
        self.position = np.clip(pos, self.bounds[0], self.bounds[1])
        if (self.position != pos).any() or np.random.normal() > self.wander:
            self.direction += np.random.uniform(-math.pi/2., math.pi/2.)
            if self.direction > 2*math.pi: self.direction -= 2*math.pi
            if self.direction < 0: self.direction += 2*math.pi
        return self.position

class Environment(object):
    def __init__(self, bounds=[], anchors=7, sensor_noise=1., nlos_noise=1., rmse=18.,
        mean_readings=4, target_speed=60., dim=2):
        self.bounds = bounds
        self.sensor_noise = sensor_noise
        self.nlos_noise = nlos_noise
        self.mse_target = rmse
        self.mean_readings = mean_readings
        self.anchors = np.random.uniform(bounds[0], bounds[1], (anchors, dim))
        self.tag = Beacon2D(bounds)
        self.target = Target(bounds=bounds, speed=target_speed)
        self.err = []
        self.rms = []
        
    def get_reading(self):
        i = np.random.choice(tuple(range(len(self.anchors))))
        r = distance(self.anchors[i], self.target.position)
        s_noise = np.random.normal(0., self.sensor_noise)
        n_noise = np.clip(np.random.logistic(-1., self.nlos_noise), 1., None)
        r_ = (r + s_noise) * n_noise
        return self.anchors[i], r_

    def run_tick(self, tick, dump):
        self.target.move()
        n = int(max(np.random.normal(self.mean_readings), 1))
        anchors, ranges = [], []
        for _ in range(n):
            a, r = self.get_reading()
            anchors.append(a)
            ranges.append(r)
        ranges = [r - min(ranges) for r in ranges]
        for anchor, rng in zip(anchors, ranges):
            self.tag.reading(anchor, rng)
        pre_anchors = self.tag.anchors()
        self.tag.update(tick, self.mse_target)
        err = distance(self.target.position, self.tag.position())
        rms = math.sqrt(self.tag.error())
        if dump:
            print "X: %s T: %s E: %-7.3f M: %-7.3f A: +%2d/%2d/%2d" % (
                    self.target.position,
                    self.tag.position(),
                    err, rms, n, pre_anchors, self.tag.anchors())
        if rms < 1e9:
            self.err.append(err)
            self.rms.append(rms)
        self.tag.clear()

    def run(self, ticks, dump):
        for t in range(ticks):
            self.run_tick(t, dump)
        return np.mean(self.err), np.std(self.err), np.mean(self.rms), np.std(self.rms)

if __name__ == "__main__":
    np.set_printoptions(precision=3)
    cfg = yaml.load(open("simulate.yaml", "r"))
    ticks = cfg.pop("runs", 10000)
    dump = cfg.pop("dump", False)
    env = Environment(**cfg)
    start = time.time()
    mean, std, mse_mean, mse_std = env.run(ticks, dump)
    elapsed = time.time() - start
    print "ticks run:", ticks
    print "mean err:", mean
    print "err std dev:", std
    print "avg RMS err:", mse_mean
    print "RMS std dev:", mse_std
    print "elapsed: %.3fs (%.6f per)" % (elapsed, elapsed / ticks)

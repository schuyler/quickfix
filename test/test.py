import numpy as np
import math, time
from quickfix import Beacon2D

def generate_anchor_positions(n, bounds, dim=3):
    return np.random.uniform(bounds[0], bounds[1], (n, dim))

def generate_tag_position(bounds, dim=3):
    return np.random.uniform(bounds[0], bounds[1], (dim,))

def compute_actual_ranges(actual, anchors):
    return np.linalg.norm(anchors - actual, axis=1)

def generate_noise(shape, factor=0.25):
    # Assume measurement noise is zero-centered Gaussian
    return np.random.normal(0.0, factor, shape)

def generate_nlos_noise(shape, factor=1.):
    # Assume NLOS noise is always positive and multiplicative
    # on a logistic distribution
    return np.clip( 
        np.random.logistic(-1., 1., shape),
        1.,
        None
    )

def distance(a, b):
    return np.linalg.norm(a - b)

def run_test(tag, actual_position, actual_ranges, bounds, noise_std=0.15, filtered=True, debug=False):
    def d(*args):
        if debug: print " ".join(map(repr, args))
    shape = actual_ranges.shape

    noise = generate_noise(shape, noise_std)
    d("     noise:", noise)
    nlos_noise = generate_nlos_noise(shape)
    d("nlos noise:", nlos_noise)
    noisy_ranges = (actual_ranges + noise) * nlos_noise
    #noisy_ranges = actual_ranges + noise

    for i, r in enumerate(noisy_ranges):
        tag.range(i, r)

    guess, mse = tag.fix(18.)
    if guess[0] == bounds[0][0] or \
       guess[1] == bounds[0][1] or \
       guess[0] == bounds[1][1] or \
       guess[1] == bounds[1][1]:
           pass
           #print "stop:", guess
           #raise

    error = distance(guess, actual_position)
    d("act:", actual_position, "upd:", guess, "err:", round(error, 6))
    return error, np.sqrt(mse)

def run_tests(m=10000, n_anchors=7, noise=0.05, debug=False):
    dim = 2
    bounds = [[-1000., -500.], [1000., 500.]]
    noise = 6.
    errors = []
    rms_errors = []

    for _ in range(m):
        anchors = generate_anchor_positions(n_anchors, bounds, dim)
        actual_position = generate_tag_position(bounds, dim)
        actual_ranges = compute_actual_ranges(actual_position, anchors)

        try:
            tag = Beacon2D(bounds)
            for i, a in enumerate(anchors):
                tag.anchor(i, a)
            err, rms_err = run_test(tag, actual_position, actual_ranges, bounds, noise, debug)
            errors.append(err)
            rms_errors.append(rms_err)
        except Exception, e:
            print "Trapped exception:", e
            raise
    errors = np.array(errors)
    rms_errors = np.array(rms_errors)
    return len(errors), errors.mean(), errors.std(), rms_errors.mean(), rms_errors.std()

if __name__ == "__main__":
    start = time.time()
    n, mean, std, rms_mean, rms_std = run_tests()
    elapsed = time.time() - start
    print "tests run:", n
    print "mean err:", mean
    print "err std dev:", std
    print "mean RMS err:", rms_mean
    print "RMS err std dev:", rms_std
    print "elapsed: %.3fs (%.6f per)" % (elapsed, elapsed / n)

import numpy as np
import math, time, yaml
from quickfix import Beacon2D

def generate_anchor_positions(n, bounds, dim=3):
    return np.random.uniform(bounds[0], bounds[1], (n, dim))

def generate_tag_position(bounds, dim=3):
    return np.random.uniform(bounds[0], bounds[1], (dim,))

def compute_actual_ranges(actual, anchors):
    return np.linalg.norm(anchors - actual, axis=1)

def generate_sensor_noise(shape, factor=0.25):
    # Assume measurement noise is zero-centered Gaussian
    return np.random.normal(0.0, factor, shape)

def generate_nlos_noise(shape, factor=1.):
    # Assume NLOS noise is always positive and multiplicative
    # on a logistic distribution
    return np.clip( 
        np.random.logistic(-1., factor, shape),
        1.,
        None
    )

def distance(a, b):
    return np.linalg.norm(a - b)

def run_test(tag, actual_position, actual_ranges, anchors, bounds,
        sensor_noise=1., nlos_noise=1., mse_target=18., debug=False):
    def d(*args):
        if debug: print " ".join(map(repr, args))
    shape = actual_ranges.shape

    s_noise = generate_sensor_noise(shape, sensor_noise)
    d("     noise:", s_noise)
    n_noise = generate_nlos_noise(shape, nlos_noise)
    d("nlos noise:", n_noise)
    noisy_ranges = (actual_ranges + s_noise) * n_noise

    for a, r in zip(anchors, noisy_ranges):
        tag.reading(a, r)

    guess, mse = tag.fix(mse_target)
    """
    if guess[0] == bounds[0][0] or \
       guess[1] == bounds[0][1] or \
       guess[0] == bounds[1][1] or \
       guess[1] == bounds[1][1]:
           pass
    """

    error = distance(guess, actual_position)
    d("act:", actual_position, "upd:", guess, "err:", round(error, 6))
    return error, np.sqrt(mse)

def run_tests(runs=10000, anchors=7, sensor_noise=6., nlos_noise=1.,
    bounds=[[-1000., -500.], [1000., 500.]], rmse=18., debug=False):
    dim = 2
    errors = []
    rms_errors = []

    for _ in range(runs):
        anchor_set = generate_anchor_positions(anchors, bounds, dim)
        actual_position = generate_tag_position(bounds, dim)
        actual_ranges = compute_actual_ranges(actual_position, anchor_set)

        try:
            tag = Beacon2D(bounds)
            err, rms_err = run_test(tag, actual_position, actual_ranges, anchor_set, bounds,
                                    sensor_noise, nlos_noise, rmse, debug)
            errors.append(err)
            rms_errors.append(rms_err)
        except Exception, e:
            print "Trapped exception:", e
            raise
    errors = np.array(errors)
    rms_errors = np.array(rms_errors)
    return len(errors), errors.mean(), errors.std(), rms_errors.mean(), rms_errors.std()

if __name__ == "__main__":
    cfg = yaml.load(open("test.yaml", "r"))
    start = time.time()
    n, mean, std, rms_mean, rms_std = run_tests(**cfg)
    elapsed = time.time() - start
    print "tests run:", n
    print "mean err:", mean
    print "err std dev:", std
    print "mean RMS err:", rms_mean
    print "RMS err std dev:", rms_std
    print "elapsed: %.3fs (%.6f per)" % (elapsed, elapsed / n)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "quickfix.h"

float get_clock_tick() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (float)t.tv_sec + ((float) t.tv_nsec /  1e9);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Supply a data file");
        return -1;
    }

    FILE *in = fopen(argv[1], "r");
    if (in == NULL) {
        printf("Can't open %s\n", argv[1]);
        return -1;
    }

    // Seed C stdlib random number generator
    srand(time(NULL));
    
    const float maxX = 1200., maxY = 800.;
    Bounds2D *bound = bounds2d_new(0., 0., maxX, maxY);

    ParticleFilter2D *filter;
#ifdef DISABLE_PARTICLE_FILTER
    qfdebug("Using no filter");
    filter = NULL;
#else
    qfdebug("Using particle filter");
    filter = particlefilter2d_new(50, 3., 3., bound);
#endif

    Beacon2D *b = beacon2d_new(bound, filter);
    float x, y, z, dd;
    const float maxError = 16.;
    const float multiPath = sqrt(maxX*maxX + maxY*maxY);
    float tick = 0.0;

    while (fscanf(in, "%f %f %f %f", &x, &y, &z, &dd) != EOF) {
        if (dd >= multiPath) {
            printf("bad: %9.3f %9.3f %9.3f\n", x, y, dd);
        } else if (dd >= 0) {
            printf("in : %9.3f %9.3f %9.3f\n", x, y, dd);
            beacon2d_reading(b, x, y, dd);
        } else {
            // If running in real time:
            //   tick = get_clock_tick();
            // Because demo:
            tick += 1.0;
            bool ok = beacon2d_update(b, tick, maxError, 100);
            float x = beacon2d_x(b),
                  y = beacon2d_y(b),
                  err = beacon2d_error(b);
            printf("%s: %9.3f %9.3f %9.3f\n----\n",
                    (ok ? "out" : "nop"),
                    x, y, sqrt(err));
            beacon2d_clear(b);
        }
    }
    beacon2d_delete(b);
}

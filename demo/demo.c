#include <stdio.h>
#include "quickfix.h"
#include "math.h"

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
    
    const float maxX = 350., maxY = 350;
    Bounds2D *bound = bounds2d_new(0., 0., maxX, maxY);
    PointFilter2D *filter = particlefilter2d_new(10, 3., 6., bound);
    Beacon2D *b = beacon2d_new(bound, filter);
    float x, y, z, dd;
    const float maxError = 100.;
    const float multiPath = sqrt(maxX*maxX + maxY+maxY);
    int tick = 0;

    while (fscanf(in, "%f %f %f %f", &x, &y, &z, &dd) != EOF) {
        if (dd >= multiPath) {
            printf("bad: %9.3f %9.3f %9.3f\n", x, y, dd);
        } else if (dd >= 0) {
            printf("in : %9.3f %9.3f %9.3f\n", x, y, dd);
            beacon2d_reading(b, x, y, dd);
        } else {
            bool ok = beacon2d_update(b, tick, maxError);
            float x = beacon2d_x(b),
                  y = beacon2d_y(b),
                  err = beacon2d_error(b);
            printf("%s: %9.3f %9.3f %9.3f\n----\n",
                    (ok ? "out" : "nop"),
                    x, y, sqrt(err));
            tick++;
        }
    }
}

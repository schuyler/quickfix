#include <stdio.h>
#include "quickfix.h"

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
    
    Beacon2D *b = Beacon2D_new(0., 0., 200., 200.);
    float x, y, z, dd;
    const float maxError = 100.;
    int tick = 0;

    while (fscanf(in, "%f %f %f %f", &x, &y, &z, &dd) != EOF) {
        if (dd >= 0) {
            printf("in : %9.3f %9.3f %9.3f\n", x, y, dd);
            Beacon2D_Reading(b, x, y, dd);
        } else {
            bool ok = Beacon2D_Update(b, tick, maxError);
            if (ok) {
                float x = Beacon2D_X(b), y = Beacon2D_Y(b);
                printf("out: %9.3f %9.3f\n", x, y);
            }
            tick++;
        }
    }
}

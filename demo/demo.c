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
    
    Beacon2D::Bounds bounds;
    bounds << 0., 0., 200., 200.;
    Beacon2D b(bounds);
    float x, y, z, dd;
    const float maxError = 100.;
    int tick = 0;

    while (fscanf(in, "%f %f %f %f", &x, &y, &z, &dd) != EOF) {
        if (dd >= 0) {
            printf("in : %9.3f %9.3f %9.3f\n", x, y, dd);
            Beacon2D::Point p;
            p << x, y;
            b.Reading(p, dd);
        } else {
            bool ok = b.Update<Beacon2D::DifferenceSolver>(tick, maxError);
            if (ok) {
                const Beacon2D::Point p = b.Position();
                printf("out: %9.3f %9.3f\n", p[0], p[1]);
            }
            tick++;
        }
    }
}

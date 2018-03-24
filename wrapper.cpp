#include "beacon.h"

typedef Beacon<float, 2> Beacon2D;

extern "C" {

    Beacon2D *Beacon2D_new() {
        return new Beacon2D();
    }

    void Beacon2D_Anchor(Beacon2D *b, AnchorID id, float x, float y) {
        Beacon2D::Point p;
        p << x, y;
        b->Anchor(id, p);
    }

    void Beacon2D_Range(Beacon2D *b, AnchorID id, float r) {
        b->Range(id, r);
    }
}

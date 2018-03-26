#include "beacon.h"
#include <iostream>

typedef Beacon<float, 2> Beacon2D;

extern "C" {
    Beacon2D *Beacon2D_new(float minX, float minY, float maxX, float maxY) {
        Beacon2D::Bounds bound;
        bound << minX, minY, maxX, maxY;
        return new Beacon2D(bound);
    }

    void Beacon2D_Anchor(Beacon2D *b, AnchorID id, float x, float y) {
        Beacon2D::Point p;
        p << x, y;
        b->Anchor(id, p);
    }

    void Beacon2D_Range(Beacon2D *b, AnchorID id, float r) {
        b->Range(id, r);
    }

    float Beacon2D_X(Beacon2D *b) {
        return b->Position()[0];
    }

    float Beacon2D_Y(Beacon2D *b) {
        return b->Position()[1];
    }

    float Beacon2D_Error(Beacon2D *b) {
        return b->Error();
    }

    void Beacon2D_Fix(Beacon2D *b, float rmsError) {
        Beacon2D estimate = b->Fix(rmsError);
        b->Position(estimate.Position());
        b->Error(estimate.Error());
    }

    bool Beacon2D_Update(Beacon2D *b, float rmsThreshold) {
        return b->Update(rmsThreshold);
    }
}

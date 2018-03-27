#include "quickfix.h"

template class Beacon<float, 2>;
// template class Beacon<float, 3>;

extern "C" {
    Beacon2D *beacon2d_new(float minX, float minY, float maxX, float maxY) {
        Beacon2D::Bounds bound;
        bound << minX, minY, maxX, maxY;
        return new Beacon2D(bound);
    }

    void beacon2d_anchor(Beacon2D *b, int id, float x, float y) {
        Beacon2D::Point p;
        p << x, y;
        b->Anchor(id, p);
    }

    void beacon2d_range(Beacon2D *b, int id, float r) {
        b->Range(id, r);
    }

    void beacon2d_reading(Beacon2D *b, float x, float y, float r) {
        Beacon2D::Point p;
        p << x, y;
        b->Reading(p, r);
    }

    void beacon2d_clear(Beacon2D *b) {
        return b->Clear();
    }

    float beacon2d_x(Beacon2D *b) {
        return b->Position()[0];
    }

    float beacon2d_y(Beacon2D *b) {
        return b->Position()[1];
    }

    float beacon2d_error(Beacon2D *b) {
        return b->Error();
    }

    int beacon2d_anchors(Beacon2D *b) {
        return b->AnchorMatrix().rows();
    }

    void beacon2d_fix(Beacon2D *b, float tick, float rmsError) {
        Beacon2D estimate = b->Fix<Beacon2D::DifferenceSolver>(tick, rmsError);
        //Beacon2D estimate = b->Fix<Beacon2D::RangeSolver>(rmsError);
        // TODO: take these out and return the values
        b->Position(estimate.Position());
        b->Error(estimate.Error());
    }

    bool beacon2d_update(Beacon2D *b, float tick, float rmsThreshold) {
        return b->Update<Beacon2D::DifferenceSolver>(tick, rmsThreshold);
        //return b->Update<Beacon2D::RangeSolver>(rmsThreshold);
    }
}

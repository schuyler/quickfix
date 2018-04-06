#include "quickfix.h"

template class Beacon<float, 2>;

extern "C" {
    Bounds2D *bounds2d_new(float minX, float minY, float maxX, float maxY) {
        Beacon2D::Bounds *bound = new Beacon2D::Bounds();
        *bound << minX, minY, maxX, maxY;
        return bound;
    }

    ParticleFilter2D *particlefilter2d_new(int n, float momentum,
                                         float dispersion, Bounds2D *bound) {
        ParticleFilter2D *filter = new ParticleFilter2D(n, momentum, dispersion, *bound);
        return filter;
    }

    Beacon2D *beacon2d_new(const Bounds2D *bound, ParticleFilter2D *filter) {
        Beacon2D *b;
        if (filter != NULL) {
            b = new Beacon2D(*bound, filter);
        } else {
            IdentityFilter<float, 2> *ifilter = new IdentityFilter<float, 2>;
            b = new Beacon2D(*bound, ifilter);
        }
        return b;
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

    void beacon2d_fix(Beacon2D *b, float tick, float rmsError, int maxTries) {
        Beacon2D estimate = b->Fix<Beacon2D::DifferenceSolver>(tick, rmsError, maxTries);
        //Beacon2D estimate = b->Fix<Beacon2D::RangeSolver>(rmsError);
        // TODO: take these out and return the values
        b->Position(estimate.Position());
        b->Error(estimate.Error());
    }

    bool beacon2d_update(Beacon2D *b, float tick, float rmsThreshold, int maxTries) {
        return b->Update<Beacon2D::DifferenceSolver>(tick, rmsThreshold, maxTries);
        //return b->Update<Beacon2D::RangeSolver>(rmsThreshold);
    }
}

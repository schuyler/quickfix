#ifndef _QUICKFIX_H
#define _QUICKFIX_H

#include "quickfix/core.h"

typedef Beacon<float, 2> Beacon2D;
typedef Beacon2D::Bounds Bounds2D;
typedef ParticleFilter<float, 2> ParticleFilter2D;

extern "C" {
    Bounds2D *bounds2d_new(float minX, float minY, float maxX, float maxY);

    ParticleFilter2D *particlefilter2d_new(int n, float dispersion,
                                         float momentum, Bounds2D *bound);

    Beacon2D *beacon2d_new(const Bounds2D *bound, const ParticleFilter2D *filter);
    void beacon2d_anchor(Beacon2D *b, int id, float x, float y);
    void beacon2d_range(Beacon2D *b, int id, float r);
    void beacon2d_reading(Beacon2D *b, float x, float y, float r);
    void beacon2d_clear(Beacon2D *b);
    float beacon2d_x(Beacon2D *b);
    float beacon2d_y(Beacon2D *b);
    float beacon2d_error(Beacon2D *b);
    int beacon2d_anchors(Beacon2D *b);
    void beacon2d_fix(Beacon2D *b, float tick, float rmsError);
    bool beacon2d_update(Beacon2D *b, float tick, float rmsThreshold);
}

#endif

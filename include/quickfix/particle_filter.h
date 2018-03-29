#ifndef _QUICKFIX_PARTICLE_FILTER_H
#define _QUICKFIX_PARTICLE_FILTER_H

#include "filter.h"
#include <cmath>

template <typename F, int D>
class ParticleFilter : public Filter<F, D> {
  public:
    typedef Array<F, 1, D> Point;
    typedef Array<F, 2, D> Bounds;
  protected:
    typedef Matrix<F, Dynamic, D> Particles;
    typedef Matrix<F, Dynamic, 1> Weights;


    Particles P;
    Weights W;
    Point X;
    F dispersion;
    F inertia;
    Bounds bound;
    int N;

    void weigh(F dT, const Point &x);
    void shuffle();
    void perturb(F dT);
  public:

    ParticleFilter(int n_, F disp, F inert, Bounds b) : Filter<F,D>(),
        N(n_), dispersion(disp), inertia(inert), bound(b) { Reset(); }

    void Reset();
    void Reset(const Point &p) { Reset(); }
    bool Update(const F dT, const Point &p);
    bool Update(const F dT);
    const Point &Position() { return X; }
};

template <typename F, int D>
void ParticleFilter<F,D>::weigh(F dT, const Point &x) {
    Weights dx = (P.rowwise() - x).norm();
    Weights w = (dx.array() / (-2. * pow(inertia * dT, 2))).exp();
    W = (w / w.sum()).matrix();
}

template <typename F, int D>
void ParticleFilter<F,D>::shuffle() {
    int i = 0, j = 0;
    Particles p_ = P;
    while (i < N) {
        while (i < N && rand() < W[j]) {
            P[i] = p_[j];
            i++;
        }
        j = (j + 1) % N;
    }
}

template <typename F, int D>
void ParticleFilter<F,D>::perturb(F dT) {
    // z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
    Array<F, 1, D> u1 = Weights::Random(1, N).array() * 0.5 + 0.5,
                   u2 = Weights::Random(1, N).array() * 0.5 + 0.5;
    Array<F, 1, D> w = (u1.log() * -2.).sqrt() * (u2 * 2 * M_PI).cos();
    W = (w * dispersion * dT).matrix();
}

template <typename F, int D>
void ParticleFilter<F,D>::Reset() {
    Array<F, 1, D> range = bound[1] - bound[0];
    Array<F, Dynamic, D> particles = (Particles::Random(N, D).array() * 0.5 + 0.5) * range;
    P = particles.matrix();
    W = Weights::Zero(1, D);
}

template <typename F, int D>
bool ParticleFilter<F,D>::Update(F dT, const Point &p) {
    weigh(dT, p);
    shuffle();
    X = P.colwise().mean();
    perturb(dT);
}

template <typename F, int D>
bool ParticleFilter<F,D>::Update(F dT) {
    // this space left intentionally blank
}

#endif

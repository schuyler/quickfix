#ifndef _QUICKFIX_PARTICLE_FILTER_H
#define _QUICKFIX_PARTICLE_FILTER_H

#include "filter.h"
#include <cmath>

template <typename F, int D>
class ParticleFilter : public FilterBase<F, D> {
  public:
    typedef typename FilterBase<F,D>::Point Point;
    typedef typename FilterBase<F,D>::Bounds Bounds;
  protected:
    typedef Matrix<F, Dynamic, D> Particles;
    typedef Matrix<F, Dynamic, 1> Weights;

    Particles P;
    Weights W;
    Point X;

    int N;
    F dispersion;
    F momentum;
    Bounds bound;

    void weigh(F dT, const Point &x);
    void shuffle();
    void perturb(F dT);
  public:
    ParticleFilter(int n_, F inert, F disp, Bounds b) : FilterBase<F,D>(),
        N(n_), dispersion(disp), momentum(inert), bound(b) { Reset(); }

    void Reset();
    void Reset(const Point &p) { Reset(); }
    bool Update(const F dT, const Point &p);
    bool Update(const F dT);
    const Point &Position() { return X; }
};

template <typename F, int D>
void ParticleFilter<F,D>::weigh(F dT, const Point &x) {
    double variance = 2. * pow(momentum * dT, 2);
    Weights dx_ = (P.rowwise() - x.matrix()).rowwise().norm();
    Matrix<double, Dynamic, 1> dx = dx_.template cast<double>();
    //std::cout << "dx: " << dx << "\n";

    Matrix<double, Dynamic, 1> w = (-dx.array() / variance).exp();
    //std::cout << "weights: " << w << "\n";
    // FIXME: check for the situation where w.sum() == 0
    W = (w / w.sum()).matrix().cast<F>();
    //std::cout << "normalized: " << W << "\n";
}

template <typename F, int D>
void ParticleFilter<F,D>::shuffle() {
    int i = 0, total = 0;
    Particles p_ = P;
    while (i < N) {
        // FIXME: this seems heavy handed
        Weights select(N, 1);
        select << Weights::Random(N, 1).array() * 0.5 + 0.5;
        select -= W;
        for (int j = 0; i < N && j < N; j++) {
            //std::cout << i << ": " << j << " " << W[j] << " > " << select[j] << "\n";
            if (select[j] < 0.) {
                P.row(i)= p_.row(j);
                i++;
            }
            total++;
        }
        //j = (j + 1) % N;
    }
    qfdebug("Particle shuffle took " << total << " iterations");
}

template <typename F, int D>
void ParticleFilter<F,D>::perturb(F dT) {
    // FIXME: this only works in 2D for now
    // z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
    Array<F, Dynamic, 1> u1 = Array<F, Dynamic, 1>::Random(N, 1) * 0.5 + 0.5,
                         u2 = Array<F, Dynamic, 1>::Random(N, 1) * 0.5 + 0.5;
    Array<F, Dynamic, 1> dx = (u1.log() * -2.).sqrt() * (u2 * 2 * M_PI).cos() * dispersion * dT,
                         dy = (u2.log() * -2.).sqrt() * (u1 * 2 * M_PI).sin() * dispersion * dT;
    Particles dP(N, D);
    dP << dx, dy;
    P = P + dP;
    // FIXME: clipping should happen here
    //P = P.cwiseMax(bound.row(0))
    //     .cwiseMin(bound.row(1));
}

template <typename F, int D>
void ParticleFilter<F,D>::Reset() {
    Array<F, 1, D> range = bound.row(1) - bound.row(0);
    // FIXME: clean all this up
    Array<F, Dynamic, D> particles(N, D);
    particles << (Particles::Random(N, D).array() * 0.5 + 0.5);
    particles.col(0) = particles.col(0) * range(0) + bound(0, 0);
    particles.col(1) = particles.col(1) * range(1) + bound(0, 1);
    P.resize(N, D);
    P << particles.matrix();
    W.resize(N, 1);
    W << Weights::Zero(N, 1);
}

template <typename F, int D>
bool ParticleFilter<F,D>::Update(F dT, const Point &p) {
    weigh(dT, p);
    shuffle();
    X = P.colwise().mean();
    perturb(dT);
    return true;
}

template <typename F, int D>
bool ParticleFilter<F,D>::Update(F dT) {
    // this space left intentionally blank
    return true;
}

#endif

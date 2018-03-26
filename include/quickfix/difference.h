#ifndef _QUICKFIX_DIFFERENCE_H
#define _QUICKFIX_DIFFERENCE_H

template <typename F, int D>
void Beacon<F, D>::DifferenceFunctor::setCoefficients() {
    int n = RangeFunctor::resizeCoefficients();
    Matrix<F, Dynamic, 1> K(n, 1);
    K = B.A.array().pow(2.0).matrix().rowwise().sum();
    G << B.A, B.dR;
    h << -(1.0 / 2.0) * (
        ((B.R.array() - B.dR[0]).pow(2.0) + K(0,0)).matrix() - K
    );
}

template <typename F, int D>
int Beacon<F,D>::DifferenceFunctor::operator()(const Point &X_hat, Ranges &fvec) const {
    Matrix<F, Dynamic, 1> x(D+1, 1);
    x << X_hat.transpose(), B.dR[0];
    fvec << (G * x - h);
    return 0;
}

#endif

#ifndef _QUICKFIX_RANGE_H
#define _QUICKFIX_RANGE_H

template <typename F, int D>
void Beacon<F, D>::RangeSolver::setCoefficients() {
    int n = B.A.rows();
    G.resize(n, D+1);
    h.resize(n, 1);
    G << B.A, Matrix<F, Dynamic, 1>::Constant(n, 1, -0.5);
    h << (1.0 / 2.0) * (
        B.A.array().pow(2.0).matrix().rowwise().sum() -
        B.R.array().pow(2.0).matrix()
    );
}

template <typename F, int D>
typename Beacon<F, D>::RangeSolver::InputType Beacon<F, D>::RangeSolver::solveLinear() {
    setCoefficients();
    return G.householderQr().solve(h);
}

template <typename F, int D>
int Beacon<F,D>::RangeSolver::operator()(const InputType &X_hat, Ranges &fvec) const {
    Point x = X_hat.transpose().leftCols(D);
    Ranges R_hat = Beacon::calculateRanges(B.A, x);
    fvec << (B.R - R_hat);
    return 0;
}

#endif

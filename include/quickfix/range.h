#ifndef _QUICKFIX_RANGE_H
#define _QUICKFIX_RANGE_H

template <typename F, int D>
void Beacon<F, D>::RangeFunctor::setCoefficients() {
    int n = B.A.rows();
    G << B.A, Matrix<F, Dynamic, 1>::Constant(n, 1, -0.5);
    h << (1.0 / 2.0) * (
        B.A.array().pow(2.0).matrix().rowwise().sum() -
        B.R.array().pow(2.0).matrix()
    );
}

template <typename F, int D>
typename Beacon<F,D>::Point Beacon<F, D>::RangeFunctor::solveLinear() {
    setCoefficients();
    HouseholderQR<MatrixXf> qr = G.householderQr();
    Matrix<F, Dynamic, 1> x = qr.solve(h);
    // G.colPivHouseholderQr().solve(h);
    // Turn the [3, 1] solution matrix into a [1, 2] vector
    Map<Point> y(x.data(), x.size() - 1);
    return y;
}

template <typename F, int D>
int Beacon<F,D>::RangeFunctor::operator()(const Point &X_hat, Ranges &fvec) const {
    Ranges R_hat = Beacon::calculateRanges(B.A, X_hat);
    fvec << (B.R - R_hat);
    return 0;
}

// https://github.com/cryos/eigen/blob/master/unsupported/test/NonLinearOptimization.cpp#L553
template <typename F, int D>
template <typename Functor>
typename Beacon<F, D>::Point Beacon<F, D>::solveNonLinear(const Beacon &B) {
    Functor functor(B);
    NumericalDiff<Functor> numDiff(functor);
    LevenbergMarquardt<NumericalDiff<Functor>, F> lm(numDiff);

    Point x0;
    if (B.Located) {
        x0 = functor.solveLinear();
    } else {
        x0 = B.X;
    }
    typename Functor::InputType x = x0.matrix().transpose();
    lm.minimize(x);
    return x;
}

#endif

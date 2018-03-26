#ifndef _QUICKFIX_SOLVER_H
#define _QUICKFIX_SOLVER_H

template <typename F, int D>
typename Beacon<F, D>::Ranges Beacon<F, D>::calculateRanges(Anchors a, Point x) {
    return (a.rowwise() - x.matrix()).rowwise().norm();
}

template <typename F, int D>
typename Beacon<F,D>::Point Beacon<F, D>::leastSquares(
        Beacon<F, D>::Anchors A,
        Beacon<F, D>::Ranges R) {
    int n = A.rows();

    Matrix<F, Dynamic, Dynamic> G(n, D+1);

    G << A, 
         Matrix<F, Dynamic, 1>::Constant(n, 1, -0.5);

    Matrix<F, Dynamic, 1> h =
        (1.0 / 2.0) * (
                A.array().pow(2.0).matrix().rowwise().sum() -
                R.array().pow(2.0).matrix()
        );

    HouseholderQR<MatrixXf> qr = G.householderQr();
    Matrix<F, Dynamic, 1> x = qr.solve(h);
    // G.colPivHouseholderQr().solve(h);
    // Turn the [3, 1] solution matrix into a [1, 2] vector
    Map<Point> y(x.data(), x.size() - 1);
    return y;
}

template <typename F, int D>
int Beacon<F,D>::NonLinearFunctor::operator()(const Point &X_hat, Ranges &fvec) const {
    Ranges R_hat = Beacon::calculateRanges(A, X_hat);
    fvec << (R - R_hat);
    return 0;
}

// https://github.com/cryos/eigen/blob/master/unsupported/test/NonLinearOptimization.cpp#L553
template <typename F, int D>
typename Beacon<F, D>::Point Beacon<F, D>::solveNonLinear(
        Beacon<F, D>::Anchors A,
        Beacon<F, D>::Ranges R) {

    Beacon::NonLinearFunctor functor(A, R);
    NumericalDiff<Beacon::NonLinearFunctor> numDiff(functor);
    LevenbergMarquardt<NumericalDiff<Beacon::NonLinearFunctor>, F> lm(numDiff);

    Point x0 = Beacon::leastSquares(A, R);
    typename Beacon::NonLinearFunctor::InputType x = x0.matrix().transpose();
    lm.minimize(x);
    return x;
}

#endif
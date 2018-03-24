#include "beacon.h"
#include <queue>

/*
template <typename F, int D>
int Beacon<F, D>::readingIndex(Beacon::Point a) {
    for (int i = 0; i < A.rows(); i++) {
        if (A.row(i).array().isApprox(a)) {
            return i;
        }
    }
    return -1;
}
*/

template <typename F, int D>
void Beacon<F, D>::Anchor(AnchorID id, Point anchor) {
    if (id >= A.rows()) {
        A.resize(id, Eigen::NoChange);
        R.resize(id);
    } 
    A.row(id) = anchor;
}

template <typename F, int D>
void Beacon<F, D>::Range(AnchorID id, F range) {
    // TODO: assert that the anchor already exists
    R(id) = range;
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
    return G.householderQr().solve(h);
}

template <typename F, int D>
typename Beacon<F, D>::Ranges Beacon<F, D>::calculateRanges(Anchors a, Point x) {
    return (a.rowwise() - x.matrix()).rowwise().norm();
}

template <typename F, int D>
F Beacon<F, D>::meanSquaredError(Ranges R_hat) {
    return (R - R_hat).squaredNorm() / R.rows(); 
}

template <typename F, int D>
Beacon<F, D>& Beacon<F, D>::Fix(F rmsError) {
    // F mseTarget = rmsError * rmsError;
    // Container queue;
    Ranges R_hat;
    X = leastSquares(A, R);
    R_hat = calculateRanges(A, X);
    Err = meanSquaredError(R_hat);
    return *this;
}

template class Beacon<float, 2>;
// template class Beacon<float, 3>;

#include "beacon.h"
#include <queue>
#include <iostream>

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
    // FIXME: this whole resize business isn't the right way to go
    if (id >= A.rows()) {
        A.conservativeResize(id + 1, Eigen::NoChange);
        R.conservativeResize(id + 1);
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

    HouseholderQR<MatrixXf> qr = G.householderQr();
    Matrix<F, Dynamic, 1> x = qr.solve(h);
    /*
    if (qr.rank() != 3) {
        std::cout << "> rank: " << qr.rank() << "\n";
    }
    */
    // G.colPivHouseholderQr().solve(h);
    // Turn the [3, 1] solution matrix into a [1, 2] vector
    Map<Point> y(x.data(), x.size() - 1);
    return y;
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
void Beacon<F, D>::estimateError() {
    Ranges R_hat;
    R_hat = calculateRanges(A, X);
    Err = meanSquaredError(R_hat);
}

template <typename F, int D>
void Beacon<F,D>::estimatePosition() {
    //X = leastSquares(A, R);
    X = solveNonLinear(A, R);
    estimateError();
}

template <typename F, int D>
void Beacon<F,D>::clipToBound() {
    X = X.max(Bound.row(0).array())
         .min(Bound.row(1).array());
    estimateError();
}

template <typename F, int D>
void Beacon<F, D>::expandAnchorSets(Beacon<F,D>::Queue &queue, F bestMse, F mseTarget) {
    int n = A.rows() - 1;
    for (int drop = 0; drop < A.rows(); drop++) {
        Anchors a(n, D);
        Ranges r(n);
        Beacon b;

        a.topRows(drop) = A.topRows(drop);
        a.bottomRows(n - drop) = A.bottomRows(n - drop);

        r.topRows(drop) = R.topRows(drop);
        r.bottomRows(n - drop) = R.bottomRows(n - drop);

        b = Beacon(Bound, a, r);
        b.estimatePosition();

        if (b.Err < bestMse) {
            queue.push(b);
        }
        if (b.Err <= mseTarget) {
            break;
        }
    }
}

template <typename F, int D>
Beacon<F, D> Beacon<F, D>::Fix(F rmsError) {
    F mseTarget = rmsError * rmsError;
    Beacon best;
    Queue queue;

    estimatePosition();
    queue.push(*this);

    while (!queue.empty()) {
        Beacon b = queue.top();
        /*
        std::cout << "- considering " << b.X <<
                     " with " << b.A.rows() << " anchors " <<
                     "(" << b.Err << " MSE)" << std::endl;
        */
        queue.pop();
        if (b.Err <= mseTarget) {
            best = b;
            break;
        }
        if (b < best) best = b;
        /*
        std::cout << "- best so far: " << b.X << " (" << b.Err << ")" << std::endl;
        */
        if (b.A.rows() > D + 1) {
            b.expandAnchorSets(queue, best.Err, mseTarget);
        }
    }
    /*
    if (best.Err < mseTarget) {
        std::cout << "= selected: " << best.X << " (" << best.Err << ")" << std::endl;
    } else {
        std::cout << "? accepted: " << best.X << " (" << best.Err << ")" << std::endl;
    }
    */
    best.clipToBound();
    return best;
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

template class Beacon<float, 2>;
// template class Beacon<float, 3>;

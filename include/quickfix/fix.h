#ifndef _QUICKFIX_FIX_H
#define _QUICKFIX_FIX_H

template <typename F, int D>
template <typename Functor>
typename Beacon<F, D>::Point Beacon<F, D>::solveNonLinear(const Beacon &B, F time) {
    Functor functor(B, time);
    functor.setCoefficients();

    NumericalDiff<Functor> numDiff(functor);
    LevenbergMarquardt<NumericalDiff<Functor>, F> lm(numDiff);

    typename Functor::InputType x = functor.solveLinear();
    lm.minimize(x);
    // Turn the [D+1, 1] solution matrix into a [1, D] vector
    Map<Point> y(x.data(), x.size() - 1);
    return y;
}

template <typename F, int D>
typename Beacon<F, D>::Ranges Beacon<F, D>::calculateRanges(const Anchors &a, const Point &x) {
    return (a.rowwise() - x.matrix()).rowwise().norm();
}

template <typename F, int D>
F Beacon<F, D>::meanSquaredError(const Ranges &R_hat) const {
    return (R - R_hat).squaredNorm() / R.rows(); 
}

template <typename F, int D>
void Beacon<F, D>::estimateError() {
    Ranges R_hat;
    int i, j;

    // FIXME FIXME: this no longer works for RangeSolver
    //
    //std::cout << "\n\nR: " << R << "\n";
    R_hat = calculateRanges(A, X);
    //std::cout << "R_hat pre: " << R_hat << "\n";
    R.minCoeff(&i, &j);
    R_hat = (R_hat.array() - R_hat[i]).matrix(); 
    //std::cout << "R_hat post: " << R_hat << "\n";
    Err = meanSquaredError(R_hat);
}

template <typename F, int D>
template <typename Solver>
void Beacon<F,D>::estimatePosition(F time) {
    X = solveNonLinear<Solver>(*this, time);
    Located = true;
    estimateError();
}

template <typename F, int D>
void Beacon<F,D>::clipToBound() {
    X = X.max(Bound.row(0).array())
         .min(Bound.row(1).array());
    estimateError();
}

template <typename F, int D>
template <typename T>
void Beacon<F, D>::dropRow(T &x, int i) {
    int n = x.rows() - 1, m = x.cols();
    x.block(i, 0, n-i, m) = x.block(i+1, 0, n-i, m);
    x.conservativeResize(n, m);
}

template <typename F, int D>
template <typename Solver>
Beacon<F, D> Beacon<F, D>::Fix(F time, F rmsError) const {
    F mseTarget = rmsError * rmsError;
    Beacon best = *this;
    Heap heap;

    best.estimatePosition<Solver>(time);
    heap.push(best);

    while (!heap.empty()) {
        Beacon b = heap.top();
        // std::cout << b.A.rows << ": " << b.X << " (" << b.Err << ")\n";
        heap.pop();
        if (b < best) best = b;
        if (b.Err <= mseTarget) break;
        // FIXME for RangeSolver: this _was_ D + 1, but Chan-Ho throws away one
        if (b.A.rows() <= D + 2) continue;

        for (int drop = 0; drop < b.A.rows(); drop++) {
            Beacon next = b;
            dropRow<Anchors>(next.A, drop);
            dropRow<Ranges>(next.R, drop);
            next.estimatePosition<Solver>(time);

            if (next < best) {
                heap.push(next);
            }
            if (next.Err <= mseTarget) {
                break;
            }
        }
    }
    best.clipToBound();
    return best;
}

template <typename F, int D>
template <typename Solver>
bool Beacon<F, D>::Update(F time, F rmsThreshold) {
    if (A.rows() < D + 1)
        return false;
    Beacon b = Fix<Solver>(time, rmsThreshold);
    if (b.Err < rmsThreshold*rmsThreshold) {
        X = b.X;
        Err = b.Err;
        Time = time;
        // FIXME: unclear when/how often we should throw away readings
        // *this = b;
        A.resize(0, 0);
        R.resize(0);
        return true;
    }
    return false;
}

#endif

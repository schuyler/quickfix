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
    // estimateError();
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
Beacon<F, D> Beacon<F, D>::Fix(F time, F rmsError, int maxTries) const {
    F mseTarget = rmsError * rmsError;
    Beacon best = *this;
    Heap heap;
    int tries;

    best.estimatePosition<Solver>(time);
    heap.push(best);

    for (tries = 0; !heap.empty() && (maxTries <= 0 || tries < maxTries);) {
        Beacon b = heap.top();
        //qfdebug("Fix: " << b.A.rows() << ": " << b.X << " (" << b.Err << ")");
        heap.pop();
        if (b.Err <= mseTarget) break;
        if (b.A.rows() <= D + 2) continue;

        for (int drop = 0; drop < b.A.rows() && (maxTries <= 0 || tries < maxTries); drop++, tries++) {
            Beacon next = b;
            dropRow<Anchors>(next.A, drop);
            dropRow<Ranges>(next.R, drop);
            next.estimatePosition<Solver>(time);
            heap.push(next);
            if (next.Err <= mseTarget) {
                b = next;
                break;
            }
        }
        if (b < best) best = b;
    }
    qfdebug("Fix took " << tries << " of " <<  maxTries << " tries using " << best.A.rows() << " of " <<  A.rows() << " anchors");
    best.clipToBound();
    return best;
}

template <typename F, int D>
template <typename Solver>
bool Beacon<F, D>::Update(F time, F rmsThreshold, int maxTries) {
    if (A.rows() <= D + 1) {
        qfdebug("Not enough readings for a fix");
        return false;
    }
    Beacon b = Fix<Solver>(time, rmsThreshold, maxTries);
    rmsThreshold *= rmsThreshold;
    b.estimateError();
    if (b.Err < rmsThreshold) {
        if (Filter != NULL) {
            Filter->Update(time-Time, b.X);
            X = Filter->Position();
        } else {
            X = b.X;
        }
        qfdebug("Δt=" << (time-Time) << ": " << X << " (" << Err << ") filtered vs " << b.X << " (" << b.Err << ") estimate on " << b.A.rows());
        clipToBound();
        Time = time;
        Err = b.Err;
        Clear();
        return true;
    } else {
        qfdebug("MSE " << b.Err << " exceeded threshold " << rmsThreshold);
        Err = b.Err;
        return false;
    }
}

#endif

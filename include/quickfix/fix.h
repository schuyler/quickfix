#ifndef _QUICKFIX_FIX_H
#define _QUICKFIX_FIX_H

template <typename F, int D>
F Beacon<F, D>::meanSquaredError(const Ranges &R_hat) const {
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
    // X = leastSquares(A,R);
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
void Beacon<F, D>::expandAnchorSets(Beacon<F,D>::Queue &queue, F bestMse, F mseTarget) const {
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
Beacon<F, D> Beacon<F, D>::Fix(F rmsError) const {
    F mseTarget = rmsError * rmsError;
    Beacon best = *this;
    Queue queue;

    best.estimatePosition();
    queue.push(best);

    while (!queue.empty()) {
        Beacon b = queue.top();
        // std::cout << b.A.rows << ": " << b.X << " (" << b.Err << ")\n";
        queue.pop();
        if (b < best) best = b;
        if (b.Err <= mseTarget) break;
        if (b.A.rows() > D + 1) {
            b.expandAnchorSets(queue, best.Err, mseTarget);
        }
    }
    best.clipToBound();
    return best;
}

template <typename F, int D>
bool Beacon<F, D>::Update(F rmsThreshold) {
    Beacon b = Fix(rmsThreshold);
    if (b.Err < rmsThreshold*rmsThreshold) {
        X = b.X;
        Err = b.Err;
        return true;
    }
    return false;
}

#endif

#ifndef _QUICKFIX_DIFFERENCE_H
#define _QUICKFIX_DIFFERENCE_H

template <typename F, int D>
void Beacon<F, D>::DifferenceSolver::setCoefficients() {
    int n = B.A.rows();

    Matrix<F, Dynamic, 1> K(n, 1);
    K = B.A.array().pow(2.0).matrix().rowwise().sum();

    Matrix<F, Dynamic, D+1> G_(n, D+1);
    G_ << B.A, B.R;

    Matrix<F, 1, D+1> G_0 = G_.row(0);
    G_ = G_.rowwise() - G_0;

    Matrix<F, Dynamic, 1> h_(n, 1);
    h_ << -(1.0 / 2.0) * (
        ((B.R.array() - B.R[0]).pow(2.0) + K(0, 0)).matrix() - K
    );

    G = G_.bottomRows(n-1);
    h = h_.bottomRows(n-1);
}

template <typename F, int D>
int Beacon<F,D>::DifferenceSolver::operator()(const InputType &X_hat, Ranges &fvec) const {
    Point x = X_hat.transpose().leftCols(D);
    int i, j;

    B.R.minCoeff(&i, &j);

    Ranges R_hat = Beacon::calculateRanges(B.A, x);
    R_hat = (R_hat.array() - R_hat[i]).matrix(); 

    fvec << (B.R - R_hat);
    return 0;
}

#endif

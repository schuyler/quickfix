#ifndef _QUICKFIX_BEACON_H
#define _QUICKFIX_BEACON_H

#include <queue>
#include <iostream>

template <typename F, int D>
int Beacon<F, D>::findRow(const Point a) {
    int i;
    for (i = 0; i < A.rows(); i++) {
        if (A.row(i).array().isApprox(a)) {
            break;
        }
    }
    return i;
}

template <typename F, int D>
void Beacon<F, D>::checkSize(int i) {
    if (i >= A.rows()) {
        A.conservativeResize(i + 1, Eigen::NoChange);
        R.conservativeResize(i + 1);
        R(i) = 0.0;
    } 
}

template <typename F, int D>
void Beacon<F, D>::Anchor(int i, Point anchor) {
    checkSize(i);
    A.row(i) = anchor;
}

template <typename F, int D>
void Beacon<F, D>::Range(int i, F range) {
    checkSize(i);
    R(i) = range;
}

template <typename F, int D>
int Beacon<F, D>::Reading(Point anchor, F range) {
    int i = findRow(anchor);
    Anchor(i, anchor);
    Range(i, range);
    return i;
}

#endif

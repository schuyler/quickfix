#ifndef _QUICKFIX_BEACON_H
#define _QUICKFIX_BEACON_H

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

#endif

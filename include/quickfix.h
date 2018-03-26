#ifndef _QUICKFIX_H
#define _QUICKFIX_H

#include <Eigen/Dense>
#include <unsupported/Eigen/NonLinearOptimization>
#include <vector>
#include <queue>
#include <limits>

using namespace Eigen;

typedef int AnchorID;

template <typename F, int D>
class Beacon {
  public:
    typedef Array<F, 1, D> Point;
    typedef Array<F, 2, D> Bounds;
    typedef Matrix<F, Dynamic, D> Anchors;
    typedef Matrix<F, Dynamic, 1> Ranges;
    typedef std::vector<Beacon> Container;
    typedef std::priority_queue<Beacon, Container, std::greater<Beacon> > Heap;

    class RangeFunctor {
      protected:
        const Beacon &B;
        Matrix<F, Dynamic, D+1> G;
        Matrix<F, Dynamic, 1> h;
        int resizeCoefficients();
        void setCoefficients();
      public:
        typedef F Scalar;
        typedef Matrix<F, Dynamic, 1> InputType;
        typedef Ranges ValueType;
        typedef Matrix<F, Dynamic, Dynamic> JacobianType;
        enum {
            InputsAtCompileTime = D,
            ValuesAtCompileTime = Dynamic
        };

        RangeFunctor(const Beacon &b) : B(b) {}
        int inputs() const { return D; }
        int values() const { return B.R.rows(); }
        int operator()(const Point &x, Ranges &fvec) const;
        Point solveLinear();
    };

    /*
    class DifferenceFunctor : RangeFunctor {
        DifferenceFunctor(const Beacon &b) : B(b) {}
    }
    */
  protected:
    Anchors A;
    Ranges R;
    Point X;
    Bounds Bound;
    F Err;
    bool Located;

    void checkSize(int i);
    int findRow(const Point &anchor) const;

    template <typename Functor>
    static Point solveNonLinear(const Beacon &b);

    static Ranges calculateRanges(const Anchors &a, const Point &x);
    F meanSquaredError(const Ranges &R_hat) const;

    void estimatePosition();
    void estimateError();
    void clipToBound();
    void expandAnchorSets(Heap &heap, F bestMse, F mseTarget) const;
  public:
    Beacon(const Bounds b) : Bound(b) { init(); }
    Beacon(const Bounds b, Anchors a, Ranges r) : A(a), R(r), Bound(b) { init(); }
    Beacon() { init(); }
    void init() {
        Err = std::numeric_limits<F>::infinity();
        Located = false;
    }
    bool operator== (const Beacon &other) const {
        return Err == other.Err;
    }
    bool operator> (const Beacon &other) const {
        return Err > other.Err;
    }
    bool operator< (const Beacon &other) const {
        return Err < other.Err;
    }

    void Anchor(int i, const Point &anchor);
    void Range(int i, F range);
    int Reading(const Point &anchor, F range);
    Beacon Fix(F rmsError) const;
    Beacon DifferenceFix(const Anchors &a, const Ranges &r, F rmsError) const;
    bool Update(F rmsThreshold);

    const Anchors &AnchorMatrix() const { return A; }
    const Ranges &RangeVector() const { return R; }
    const Point &Position() const { return X; }
    void Position(const Point &point) { X = point; }
    F Error() { return Err; }
    void Error(F value) { Err = value; }
};

#include "quickfix/beacon.h"
#include "quickfix/range.h"
#include "quickfix/fix.h"

template class Beacon<float, 2>;
template class Beacon<float, 3>;

typedef Beacon<float, 2> Beacon2D;
typedef Beacon<float, 3> Beacon3D;

#endif

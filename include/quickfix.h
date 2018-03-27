#ifndef _QUICKFIX_H
#define _QUICKFIX_H

#include <Eigen/Dense>
#include <unsupported/Eigen/NonLinearOptimization>
#include <vector>
#include <queue>
#include <limits>

using namespace Eigen;

template <typename F, int D>
class Beacon {
  public:
    typedef Array<F, 1, D> Point;
    typedef Array<F, 2, D> Bounds;
    typedef Matrix<F, Dynamic, D> Anchors;
    typedef Matrix<F, Dynamic, 1> Ranges;
    typedef std::vector<Beacon> Container;
    typedef std::priority_queue<Beacon, Container, std::greater<Beacon> > Heap;

    struct RangeSolver {
        const Beacon &B;
        Matrix<F, Dynamic, D+1> G;
        Matrix<F, Dynamic, 1> h;
        F T;
        virtual void setCoefficients();

        typedef F Scalar;
        typedef Matrix<F, Dynamic, 1> InputType;
        typedef Ranges ValueType;
        typedef Matrix<F, Dynamic, Dynamic> JacobianType;
        enum {
            InputsAtCompileTime = D,
            ValuesAtCompileTime = Dynamic
        };

        RangeSolver(const Beacon &b, F time) : B(b), T(time) {}
        int inputs() const { return D; }
        virtual int values() const { return B.R.rows(); }
        virtual int operator()(const InputType &x, Ranges &fvec) const;
        InputType solveLinear();
    };

    struct DifferenceSolver : RangeSolver {
        using RangeSolver::B;
        using RangeSolver::G;
        using RangeSolver::h;
        using typename RangeSolver::InputType;

        void setCoefficients();
        DifferenceSolver(const Beacon &b, F time) : RangeSolver(b, time) {}
        int operator()(const InputType &x, Ranges &fvec) const;
    };

  protected:
    Anchors A;
    Ranges R;
    Point X;
    Bounds Bound;
    F Err;
    F Time;
    bool Located;

    void checkSize(int i);
    int findRow(const Point &anchor) const;

    template <typename Solver>
    static Point solveNonLinear(const Beacon &b, F time);

    static Ranges calculateRanges(const Anchors &a, const Point &x);
    F meanSquaredError(const Ranges &R_hat) const;

    template <typename T>
    static void dropRow(T &x, int i);

    template <typename Solver>
    void estimatePosition(F time);
    void estimateError();
    void clipToBound();
  public:
    Beacon(const Bounds b) : Bound(b) { init(); }
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

    template <typename Solver>
    Beacon Fix(F time, F rmsError) const;

    template <typename Solver>
    bool Update(F time, F rmsThreshold);

    const Anchors &AnchorMatrix() const { return A; }
    const Ranges &RangeVector() const { return R; }
    const Point &Position() const { return X; }
    void Position(const Point &point) { X = point; }
    F Error() { return Err; }
    void Error(F value) { Err = value; }
};

#include "quickfix/beacon.h"
#include "quickfix/range.h"
#include "quickfix/difference.h"
#include "quickfix/fix.h"

template class Beacon<float, 2>;
template class Beacon<float, 3>;

typedef Beacon<float, 2> Beacon2D;
typedef Beacon<float, 3> Beacon3D;

#endif

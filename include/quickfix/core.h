#ifndef _QUICKFIX_CORE_H
#define _QUICKFIX_CORE_H

#ifdef NDEBUG
#define qfdebug(x)
#else
#include <iostream>
#define qfdebug(x) std::cout << x << std::endl
#endif

#include <Eigen/Dense>
#include <unsupported/Eigen/NonLinearOptimization>
#include <vector>
#include <queue>
#include <limits>

using namespace Eigen;

#include "particle_filter.h"

template <typename F, int D>
class Beacon {
  public:
    typedef Array<F, 1, D> Point;
    typedef Array<F, 2, D> Bounds;
    typedef Matrix<F, Dynamic, D> Anchors;
    typedef Matrix<F, Dynamic, 1> Ranges;
    typedef std::vector<Beacon> Container;
    typedef std::priority_queue<Beacon, Container, std::greater<Beacon> > Heap;
    typedef FilterBase<F, D> PointFilter;

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
    PointFilter *Filter;

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
    Beacon(const Bounds b, PointFilter *f, F t=0.) : Bound(b), Time(t), Filter(f) {
        init();
    }
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
    void Clear(void);

    template <typename Solver>
    Beacon Fix(F time, F rmsError, int maxTries) const;

    template <typename Solver>
    bool Update(F time, F rmsThreshold, int maxTries);

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

#endif

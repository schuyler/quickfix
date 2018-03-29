#ifndef _QUICKFIX_FILTER_H
#define _QUICKFIX_FILTER_H

#include <Eigen/Dense>

using namespace Eigen;

template <typename F, int D>
class FilterBase {
  public:
    typedef Array<F, 1, D> Point;
    typedef Array<F, 2, D> Bounds;

    // FIXME: these methods should be pure virtual
    virtual void Reset() {}
    virtual void Reset(const Point &p) {}
    virtual bool Update(const F dT, const Point &p) {return true;}
    virtual bool Update(const F dT) {return true;}
    virtual const Point &Position() {return *(new Point());}
};

template <typename F, int D>
class IdentityFilter : public FilterBase<F,D> {
    typedef typename FilterBase<F,D>::Point Point;
    const Point &X;
  public:
    IdentityFilter() {}
    void Reset() {}
    void Reset(const Point &p) { X = p; }
    inline bool Update(const F dT, const Point &p) { X = p; }
    bool Update(const F dT) {};
    inline const Point &Position() { return X; }
};

#endif

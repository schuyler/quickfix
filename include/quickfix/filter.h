#ifndef _QUICKFIX_FILTER_H
#define _QUICKFIX_FILTER_H

#include <Eigen/Dense>

using namespace Eigen;

template <typename F, int D>
class Filter {
  public:
    typedef Array<F, 1, D> Point;
    typedef Array<F, 2, D> Bounds;

    virtual void Reset();
    virtual void Reset(const Point &p);
    virtual bool Update(const F dT, const Point &p);
    virtual bool Update(const F dT);
    virtual const Point &Position();
};

#endif

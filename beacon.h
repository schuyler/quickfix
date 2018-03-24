#include <Eigen/Dense>

using namespace Eigen;

typedef int AnchorID;

template <typename F, int D>
class Beacon {
  public:
    typedef Array<F, 1, D> Point;
    typedef Array<F, 2, D> Bounds;
    typedef Matrix<F, Dynamic, D> Anchors;
    typedef Matrix<F, Dynamic, 1> Ranges;
    typedef std::vector<Beacon<F,D> > Container;

  protected:
    Anchors A;
    Ranges R;
    Point X;
    Bounds Bound;
    F Err;

    // int readingIndex(Point a);
    static Point leastSquares(Anchors a, Ranges r);
    static Ranges calculateRanges(Anchors a, Point x);
    F meanSquaredError(Ranges R_hat);

  public:
    Beacon(const Bounds b) { Bound = b; }
    Beacon() {}

    bool operator< (Beacon<F, D> other) {
        // NB: Ordinarily the sense of this would be < but we want _lower_
        // error items to come up first in the priority queue.
        return Err > other.Err;
    }

    Beacon<F,D> &Fix(F rmsError);
    void Anchor(AnchorID id, Point anchor);
    void Range(AnchorID id, F range);

    const Anchors AnchorSet() { return A; }
    const Ranges RangeSet() { return R; }
    const Point Position() { return X; }
    F Error() { return Err; }
};

typedef Beacon<float, 2> Beacon2D;
// typedef Beacon<float, 3> Beacon3D;

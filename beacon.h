#include <Eigen/Dense>
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
    typedef std::priority_queue<Beacon, Container, std::greater<Beacon> > Queue;

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
    void estimatePosition();
    void estimateError();
    void clipToBound();
    void expandAnchorSets(Queue &queue, F bestMse, F mseTarget);
  public:
    // TODO: Tidy up the constructors
    Beacon(const Bounds b) : Bound(b) { init(); }
    Beacon(const Bounds b, Anchors a, Ranges r) : A(a), R(r), Bound(b) { init(); }
    Beacon() { init(); }
    void init() {
        Err = std::numeric_limits<F>::infinity();
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

    Beacon Fix(F rmsError);
    void Anchor(AnchorID id, Point anchor);
    void Range(AnchorID id, F range);

    const Anchors AnchorMatrix() { return A; }
    const Ranges RangeVector() { return R; }
    const Point Position() { return X; }
    void Position(const Point point) { X = point; }
    F Error() { return Err; }
    void Error(F value) { Err = value; }
};

typedef Beacon<float, 2> Beacon2D;
// typedef Beacon<float, 3> Beacon3D;

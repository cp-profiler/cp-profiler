#ifndef CPPROFILER_TREE_SHAPE
#define CPPROFILER_TREE_SHAPE

#include "../utils/array.hh"
#include <ostream>
#include <initializer_list>


namespace cpprofiler { namespace tree {

class Extent {
public:
    /// Left extent
    int l;
    /// Right extent
    int r;
    /// Construct with \a l and \a r
    Extent(int l, int r);

    Extent() = default;

    ~Extent();
};

struct BoundingBox {
    int left;
    int right;

    int width() const {
        return right - left;
    }
};


class Shape {

    utils::Array<Extent> m_extents;

    BoundingBox m_bb;

    /// two integers per level

    // simple array:
        // int for size
        // ptr to memory 12 

    friend std::ostream& operator<<(std::ostream&, const Shape&);

public:
    Shape(int depth);
    ~Shape();

    Shape& operator=(const Shape& s);

    Shape(std::initializer_list<Extent> init_list)
    : m_extents{init_list} {}

    Shape(std::initializer_list<Extent> init_list, const BoundingBox& bb)
    : m_extents{init_list}, m_bb{bb} {
    }

    /// returns the height of the shape
    int depth() const;

    Extent& operator[](int depth);

    const Extent& operator[](int depth) const;

    void setBoundingBox(BoundingBox bb);
    const BoundingBox& boundingBox() const;

    static Shape leaf;
    static Shape hidden;

};

class ShapeDeleter {
public:
    void operator() (Shape* s) {
        if (s != &Shape::leaf && s != &Shape::hidden) {
            delete s;
        }
    }
};

}}


#endif
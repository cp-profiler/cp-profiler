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


class Shape {

    utils::Array<Extent> m_extents;

    /// two integers per level

    // simple array:
        // int for size
        // ptr to memory 12 

    friend std::ostream& operator<<(std::ostream&, const Shape&);

public:
    Shape(int depth);

    Shape(std::initializer_list<Extent> init_list)
    : m_extents{init_list} {}

    /// returns the depth of the shape
    int depth() const;

    Extent& operator[](int depth);

    const Extent& operator[](int depth) const;

    static Shape leaf;

};

}}


#endif
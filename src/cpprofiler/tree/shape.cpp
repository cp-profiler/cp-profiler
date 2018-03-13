#include "shape.hh"
#include "../config.hh"
#include <QDebug>
#include <ostream>

namespace cpprofiler { namespace tree {

    Extent::Extent(int l0, int r0) : l(l0), r(r0) {
    }

    Extent::~Extent() {
    }

}}


namespace cpprofiler { namespace tree {


    Shape::Shape(int depth) : m_extents(depth) {
        // qDebug() << "Shape()";
    }

    Shape::~Shape() {

    }

    Shape& Shape::operator=(const Shape& s) {
        m_extents = s.m_extents;
        m_bb = s.m_bb;
        return *this;
    }

    std::ostream& operator<<(std::ostream& os, const cpprofiler::tree::Shape& s) {
        os << "{ depth: " << s.depth() << ", [ ";
        
        for (auto i = 0; i < s.m_extents.size(); ++i) {
            os << "{" << s.m_extents[i].l << ":" << s.m_extents[i].r << "} ";
        }
        
        return os << "]}";
    }

    int Shape::depth() const {
        return m_extents.size();
    }

    Extent& Shape::operator[](int depth) {
        return m_extents[depth];
    }

    const Extent& Shape::operator[](int depth) const {
        return m_extents[depth];
    }

    void Shape::setBoundingBox(BoundingBox bb) {
        m_bb = bb;
    }

    const BoundingBox& Shape::boundingBox() const {
        return m_bb;
    }

    using namespace traditional;

    Shape Shape::leaf({{-HALF_NODE_WIDTH, HALF_NODE_WIDTH}}, BoundingBox{-HALF_NODE_WIDTH, HALF_NODE_WIDTH});

    Shape Shape::hidden({ {-HALF_NODE_WIDTH, HALF_NODE_WIDTH},{-NODE_WIDTH, NODE_WIDTH} },
                        BoundingBox{-NODE_WIDTH, NODE_WIDTH});
}}

#include "shape.hh"
#include <QDebug>
#include <ostream>

namespace cpprofiler { namespace tree {

    Extent::Extent(int l0, int r0) : l(l0), r(r0) {
        qDebug() << "Extent()";
    }

    Extent::~Extent() {
        qDebug() << "~Extent()";
    }

}}


namespace cpprofiler { namespace tree {


    Shape::Shape(int depth) : m_extents(depth) {

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

    Shape Shape::leaf{{-10, 10}};
}}

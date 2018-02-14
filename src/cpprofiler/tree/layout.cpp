#include "layout.hh"
#include "shape.hh"


#include <QDebug>

#include "../utils/std_ext.hh"

namespace cpprofiler { namespace tree {

    const Shape& Layout::getShape_unprotected(NodeID nid) const {

        return *m_shape_map.at(nid);
    }

    void Layout::setShape_unprotected(NodeID nid, Shape* shape) {
        m_shape_map[nid] = shape;
    }

    void Layout::setShape_unprotected(NodeID nid, std::unique_ptr<Shape> shape) {

        qDebug() << "set shape for nid: " << nid;

        // for now assume that shape cannot be reassigned
        if (m_shape_map.find(nid) != m_shape_map.end()) throw;

        m_shapes.push_back(std::move(shape));
        m_shape_map[nid] = m_shapes.back().get();
    }

    Layout::Layout() {
    }

    Layout::~Layout() = default;

    std::unique_ptr<LayoutComputer> Layout::createComputer(const Structure& str) {

        return utils::make_unique<LayoutComputer>(str, *this, m_layout_mutex);
    }

}}


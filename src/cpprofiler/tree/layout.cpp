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
        /// always check if the node had a unique shape associated with it
        /// if yes, remove it
        {
            auto it = m_shapes.find(nid);
            if (it != m_shapes.end()) {
                m_shapes.erase(it);
            }
        }

    }

    void Layout::setChildOffset_unprotected(NodeID nid, double offset) {
        m_child_offsets[nid] = offset;
    }

    void Layout::setShape_unprotected(NodeID nid, std::unique_ptr<Shape> shape) {

        setShape_unprotected(nid, shape.get());

        m_shapes[nid] = std::move(shape);
    }

    Layout::Layout() {
    }

    Layout::~Layout() = default;

    std::unique_ptr<LayoutComputer> Layout::createComputer(const Structure& str) {

        return utils::make_unique<LayoutComputer>(str, *this, m_layout_mutex);
    }

    double Layout::getOffset(NodeID nid) const {
        QMutexLocker locker(&m_layout_mutex);
        return m_child_offsets.at(nid);
    }

}}


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

    QMutex& Layout::getMutex() {
        return m_layout_mutex;
    }

    void Layout::setChildOffset_unprotected(NodeID nid, double offset) {
        m_child_offsets[nid] = offset;
    }

    void Layout::setShape_unprotected(NodeID nid, std::unique_ptr<Shape> shape) {

        setShape_unprotected(nid, shape.get());

        m_shapes[nid] = std::move(shape);
    }

    void Layout::setLayoutDone_unprotected(NodeID nid, bool val) {
        m_layout_done[nid] = val;
    }

    bool Layout::getLayoutDone_unprotected(NodeID nid) const {
        if (m_layout_done.find(nid) == m_layout_done.end()) {
            return false;
        }
        return m_layout_done.at(nid);
    }

    Layout::Layout() {
    }

    Layout::~Layout() = default;

    double Layout::getOffset(NodeID nid) const {
        QMutexLocker locker(&m_layout_mutex);

        auto it = m_child_offsets.find(nid);

        if (it == m_child_offsets.end()) {
            return 0;
        }

        return m_child_offsets.at(nid);
    }

    int Layout::getDepth(NodeID nid) const {
        return getShape_unprotected(nid).depth();
    }

    const BoundingBox& Layout::getBoundingBox(NodeID nid) const {
        QMutexLocker locker(&m_layout_mutex);

        return getShape_unprotected(nid).boundingBox();
    }

    bool Layout::getLayoutDone(NodeID nid) const {
        QMutexLocker locker(&m_layout_mutex);
        return getLayoutDone_unprotected(nid);
    }


}}


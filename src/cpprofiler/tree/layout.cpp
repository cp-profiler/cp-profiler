#include "layout.hh"
#include "shape.hh"


#include <QDebug>

#include "../utils/std_ext.hh"

namespace cpprofiler { namespace tree {

    const Shape& Layout::getShape_unsafe(NodeID nid) const {

        // if (m_shapes.size() <= nid) {
        //     m_shapes.resize(static_cast<int>(nid)+1);
        // }

        return *m_shapes[nid];
    }

    void Layout::setShape_unsafe(NodeID nid, std::unique_ptr<Shape, ShapeDeleter> shape) {

        if (m_shapes.size() <= nid) {
            m_shapes.resize(static_cast<int>(nid)+1);
        }

        m_shapes[nid] = std::move(shape);
    }

    QMutex& Layout::getMutex() {
        return m_layout_mutex;
    }

    void Layout::setChildOffset_unsafe(NodeID nid, double offset) {

        if (m_child_offsets.size() <= nid) {
            m_child_offsets.resize(static_cast<int>(nid)+1);
        }

        m_child_offsets[nid] = offset;
    }


    void Layout::setLayoutDone_unsafe(NodeID nid, bool val) {

        if (m_layout_done.size() <= nid) {
            m_layout_done.resize(static_cast<int>(nid)+1);
        }

        m_layout_done[nid] = val;
    }

    bool Layout::getLayoutDone_unsafe(NodeID nid) const {

        return m_layout_done.at(nid);
    }

    Layout::Layout() {}

    Layout::~Layout() = default;

    double Layout::getOffset(NodeID nid) const {
        QMutexLocker locker(&m_layout_mutex);

        if (m_child_offsets.size() <= nid) {
            return 0;
        }

        return m_child_offsets[nid];
    }

    int Layout::getDepth(NodeID nid) const {
        QMutexLocker locker(&m_layout_mutex);
        return getShape_unsafe(nid).depth();
    }

    const BoundingBox& Layout::getBoundingBox(NodeID nid) const {
        QMutexLocker locker(&m_layout_mutex);
        return getShape_unsafe(nid).boundingBox();
    }

    bool Layout::getLayoutDone(NodeID nid) const {
        QMutexLocker locker(&m_layout_mutex);
        return getLayoutDone_unsafe(nid);
    }

    void Layout::growDataStructures(int n_nodes) {
        QMutexLocker locker(&m_layout_mutex);
        auto old_size = m_child_offsets.size();
        m_child_offsets.resize(old_size + n_nodes);
    }


}}


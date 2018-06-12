#include "layout.hh"

#include <QDebug>
#include <iostream>

#include "../utils/std_ext.hh"

namespace cpprofiler
{
namespace tree
{

void Layout::setShape(NodeID nid, std::unique_ptr<Shape, ShapeDeleter> shape)
{
    m_shapes[nid] = std::move(shape);
}

utils::Mutex &Layout::getMutex() const
{
    return m_layout_mutex;
}

Layout::Layout()
{
}

Layout::~Layout() = default;

int Layout::getHeight(NodeID nid) const
{
    return getShape(nid)->height();
}

bool Layout::ready(NodeID nid) const
{
    return (m_layout_done.size() > nid);
}

bool Layout::getLayoutDone(NodeID nid) const
{
    if (m_layout_done.size() <= nid)
        return false;
    return m_layout_done[nid];
}

void Layout::growDataStructures(int n_nodes)
{

    if (n_nodes > m_shapes.size())
    {
        m_child_offsets.resize(n_nodes, 0);
        m_shapes.resize(n_nodes); /// should new entires be initialized?
        m_layout_done.resize(n_nodes, false);
        m_dirty.resize(n_nodes, true);
    }
}

} // namespace tree
} // namespace cpprofiler

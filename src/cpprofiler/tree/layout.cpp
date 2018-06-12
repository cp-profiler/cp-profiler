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

void Layout::setLayoutDone(NodeID nid, bool val)
{
    if (nid < m_layout_done.size())
    {
        m_layout_done[nid] = val;
    }
}

bool Layout::getLayoutDone(NodeID nid) const
{
    if (nid >= m_layout_done.size())
    {
        return false;
    }
    return m_layout_done.at(nid);
}

Layout::Layout()
{
}

Layout::~Layout() = default;

int Layout::getHeight(NodeID nid) const
{
    return getShape(nid)->height();
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

bool Layout::isDirty(NodeID nid) const
{
    if (nid >= m_dirty.size())
    {

        static int misses = 0;
        print("is dirty missed: {}", misses);
        return true;
    }
    return m_dirty[nid];
}

void Layout::setDirty(NodeID nid, bool val)
{
    if (nid < m_dirty.size())
    {
        m_dirty[nid] = val;
    }
}

} // namespace tree
} // namespace cpprofiler

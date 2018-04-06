#include "layout_computer.hh"
#include "layout.hh"
#include "structure.hh"
#include "node_tree.hh"
#include "shape.hh"
#include "../utils/std_ext.hh"
#include "../utils/perf_helper.hh"

#include "cursors/layout_cursor.hh"
#include "cursors/nodevisitor.hh"

#include <QMutex>
#include <QDebug>
#include <iostream>

namespace cpprofiler { namespace tree {

LayoutComputer::LayoutComputer(const NodeTree& tree, Layout& layout, const VisualFlags& nf)
: m_tree(tree), m_layout(layout), m_vis_flags(nf)
{

}

void LayoutComputer::setDirty(NodeID nid) {

    auto& tree_mutex = m_tree.treeMutex();
    auto& layout_mutex = m_layout.getMutex();

    m_layout.setDirty_unsafe(nid, true);
}


void LayoutComputer::dirtyUp(NodeID nid) {

    auto& tree_mutex = m_tree.treeMutex();
    auto& layout_mutex = m_layout.getMutex();

    while (nid != NodeID::NoNode) {
        m_layout.setDirty_unsafe(nid, true);
        nid = m_tree.getParent_safe(nid);
    }

}

bool LayoutComputer::compute() {

    if (!m_needs_update) {
        return false;
    }

    auto& tree_mutex = m_tree.treeMutex();
    auto& layout_mutex = m_layout.getMutex();

    // auto tree_locked = tree_mutex.tryLock();
    // auto layout_locked = layout_mutex.tryLock();

    // perfHelper.begin("layout");
    LayoutCursor lc(m_tree.getRoot(), m_tree, m_vis_flags, m_layout);
    PostorderNodeVisitor<LayoutCursor>(lc).run();
    // perfHelper.end();

    // perfHelper.begin("layout: actually compute");


    m_needs_update = false;

    // perfHelper.end();

    return true;

}

void LayoutComputer::markAsOutdated() {
    m_needs_update = true;
}


}}
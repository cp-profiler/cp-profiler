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
#include <thread>
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

    while (nid != NodeID::NoNode && !m_layout.isDirty_unsafe(nid)) {
        m_layout.setDirty_unsafe(nid, true);
        nid = m_tree.getParent(nid);
    }

}


bool LayoutComputer::compute() {

    /// do nothing if there is no nodes

    if (m_tree.nodeCount() == 0) return false;

    static bool done = false;

    if (!done) {
        debug("thread") << "LayoutComputer:compute thread:" << std::this_thread::get_id() << std::endl;
        done = true;
    }

    /// TODO: come back here (ensure mutexes work correctly)
    utils::DebugMutexLocker tree_lock(&m_tree.treeMutex());
    utils::DebugMutexLocker layout_lock(&m_layout.getMutex());

    perfHelper.begin("layout");
    LayoutCursor lc(m_tree.getRoot(), m_tree, m_vis_flags, m_layout);
    PostorderNodeVisitor<LayoutCursor>(lc).run();
    perfHelper.end();

    // perfHelper.begin("layout: actually compute");

    // perfHelper.end();


    static int counter = 0;
    if (++counter % 1 == 0) {
        // std::cerr << "computed layout " << counter * 1  << " times\n";
    }

    return true;

}


}}
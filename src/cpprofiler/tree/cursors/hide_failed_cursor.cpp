#include "hide_failed_cursor.hh"
#include "../node_tree.hh"
#include "../visual_flags.hh"
#include "../layout_computer.hh"

namespace cpprofiler { namespace tree {

HideFailedCursor::HideFailedCursor(NodeID start, const NodeTree& nt, VisualFlags& vf, LayoutComputer& lc)
: NodeCursor(start, nt), m_vf(vf), m_lc(lc)
{

}

bool HideFailedCursor::mayMoveDownwards() const {
    /// TODO:optimise this

    bool ok = NodeCursor::mayMoveDownwards() &&
           (m_tree.hasSolvedChildren(m_cur_node) || m_tree.hasOpenChildren(m_cur_node));

    return ok;
}

void HideFailedCursor::processCurrentNode() {

    /// TODO: check if has open children
    if (!m_tree.hasSolvedChildren(m_cur_node) &&
        !m_tree.hasOpenChildren(m_cur_node) &&
        m_tree.childrenCount(m_cur_node) > 0)
    {
        m_vf.set_hidden(m_cur_node, true);
        m_lc.dirtyUp(m_cur_node);

    }
}


}}
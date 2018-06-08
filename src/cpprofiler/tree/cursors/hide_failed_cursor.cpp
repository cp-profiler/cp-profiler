#include "hide_failed_cursor.hh"
#include "../node_tree.hh"
#include "../visual_flags.hh"
#include "../layout_computer.hh"

namespace cpprofiler
{
namespace tree
{

HideFailedCursor::HideFailedCursor(NodeID start, const NodeTree &nt, VisualFlags &vf, LayoutComputer &lc, bool dirty, bool &mod)
    : NodeCursor(start, nt), m_vf(vf), m_lc(lc), modified(mod), m_onlyDirty(dirty)
{
    print("Hide Failed Cursor");
}

bool HideFailedCursor::mayMoveDownwards() const
{

    bool ok = NodeCursor::mayMoveDownwards() &&
              (!m_onlyDirty || m_lc.isDirty(m_cur_node)) &&
              (m_tree.hasSolvedChildren(m_cur_node) || m_tree.hasOpenChildren(m_cur_node)) &&
              !m_vf.isHidden(m_cur_node);

    return ok;
}

inline static bool should_hide(const NodeTree &nt, const VisualFlags &vf, NodeID n)
{
    return !nt.hasSolvedChildren(n) && // does not have solution children
           !nt.hasOpenChildren(n) &&   // does not have open children
           nt.childrenCount(n) > 0 &&  // not a leaf node
           !vf.isHidden(n);            // the node is not already hidden
}

void HideFailedCursor::processCurrentNode()
{
    print("considering hiding node {}", m_cur_node);
    if (should_hide(m_tree, m_vf, m_cur_node))
    {
        modified = true;
        m_vf.setHidden(m_cur_node, true);
        m_lc.dirtyUp(m_cur_node);
    }
}

} // namespace tree
} // namespace cpprofiler
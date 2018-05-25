#include "hide_not_highlighted_cursor.hh"
#include "../visual_flags.hh"
#include "../layout_computer.hh"
#include "../node_tree.hh"

namespace cpprofiler
{

namespace tree
{

HideNotHighlightedCursor::HideNotHighlightedCursor(NodeID startNode,
                                                   const NodeTree &tree,
                                                   VisualFlags &vf,
                                                   LayoutComputer &lc)
    : NodeCursor(startNode, tree), vf_(vf), lc_(lc)
{
    //
}

void HideNotHighlightedCursor::processCurrentNode()
{
    const auto n = node();

    bool on_hp = vf_.isHighlighted(n);

    if (!on_hp)
    {
        /// find a child that is on highlighted path
        for (auto alt = 0; alt < m_tree.childrenCount(n); ++alt)
        {
            const auto child = m_tree.getChild(n, alt);
            if (onHighlightPath.contains(child))
            {
                on_hp = true;
                break;
            }
        }
    }

    if (on_hp)
    {
        onHighlightPath[n] = true;
    }
    else
    {
        vf_.setHidden(n, true);
        lc_.dirtyUp(n);
    }
}

bool HideNotHighlightedCursor::mayMoveDownwards() const
{
    const auto n = node();
    return NodeCursor::mayMoveDownwards() &&
           !vf_.isHighlighted(n) && !vf_.isHidden(n);
}

} // namespace tree
} // namespace cpprofiler
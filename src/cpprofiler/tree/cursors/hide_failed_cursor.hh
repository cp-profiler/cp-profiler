#pragma once

#include "node_cursor.hh"

namespace cpprofiler
{
namespace tree
{

class VisualFlags;
class LayoutComputer;

class HideFailedCursor : public NodeCursor
{

    VisualFlags &m_vf;
    LayoutComputer &m_lc;

    /// whether a the cursor modified the visualisation in any way
    bool &modified;
    /// whether only dirty nodes should be hidden
    bool m_onlyDirty;

  public:
    HideFailedCursor(NodeID start, const NodeTree &nt, VisualFlags &vf, LayoutComputer &lc, bool onlyDirty, bool &mod);

    /// Test if the cursor may move to the first child node
    bool mayMoveDownwards() const;
    /// Process node
    void processCurrentNode();
};

} // namespace tree
} // namespace cpprofiler
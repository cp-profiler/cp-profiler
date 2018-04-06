#pragma once

#include "node_cursor.hh"

namespace cpprofiler { namespace tree {

    class VisualFlags;
    class LayoutComputer;

class HideFailedCursor : public NodeCursor {

    VisualFlags& m_vf;
    LayoutComputer& m_lc;

public:

    HideFailedCursor(NodeID start, const NodeTree& nt, VisualFlags& vf, LayoutComputer& lc);

    /// Test if the cursor may move to the first child node
    bool mayMoveDownwards() const;
    /// Process node
    void processCurrentNode();
};


}}
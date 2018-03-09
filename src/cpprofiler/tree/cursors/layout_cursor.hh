#ifndef CPPROFILER_TREE_CURSORS_LAYOUT_CURSOR
#define CPPROFILER_TREE_CURSORS_LAYOUT_CURSOR

#include <vector>
#include "node_cursor.hh"

namespace cpprofiler { namespace tree {

    class Layout;
    class NodeFlags;

class LayoutCursor : public UnsafeNodeCursor {

    Layout& m_layout;
    const NodeTree& m_nt;
    const Structure& m_tree;
    const NodeFlags& m_node_flags;

public:
    // Constructor
    LayoutCursor(NodeID start, const NodeTree& tree, const NodeFlags& nf, Layout& lo);

    void computeForNode(NodeID nid);

    bool mayMoveDownwards();

    void processCurrentNode();

    void finalize();

};

}}


#endif
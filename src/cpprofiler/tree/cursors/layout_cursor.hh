#ifndef CPPROFILER_TREE_CURSORS_LAYOUT_CURSOR
#define CPPROFILER_TREE_CURSORS_LAYOUT_CURSOR

#include <vector>
#include "node_cursor.hh"

namespace cpprofiler { namespace tree {

class LayoutCursor : public NodeCursor {

    std::vector<NodeID>& m_layout_order;

public:
    // Constructor
    LayoutCursor(NodeID start, const NodeTree& tree, std::vector<NodeID>& order);

    bool mayMoveDownwards();

    void processCurrentNode();

    void finalize();

};

}}


#endif
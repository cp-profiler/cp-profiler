
#include "layout_cursor.hh"
#include "../node_id.hh"
#include <iostream>

namespace cpprofiler { namespace tree {

    LayoutCursor::LayoutCursor(NodeID start, const NodeTree& tree, std::vector<NodeID>& order)
        : NodeCursor(start, tree), m_layout_order(order) {}

    bool LayoutCursor::mayMoveDownwards() {
        return NodeCursor::mayMoveDownwards();
    }

    void LayoutCursor::processCurrentNode() {
        // m_layout_order.push_back(m_cur_node);
    }

    void LayoutCursor::finalize() {

    }


}}
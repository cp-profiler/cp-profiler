#ifndef CPPROFILER_TREE_CURSOR_NODE_CURSOR_HH
#define CPPROFILER_TREE_CURSOR_NODE_CURSOR_HH

#include "../node_id.hh"

namespace cpprofiler { namespace tree {

class Structure;
class NodeInfo;
class NodeTree;

class NodeCursor {

protected:

    const Structure& m_tree;
    const NodeInfo& m_node_info;
    const NodeID m_start_node;
    NodeID m_cur_node;
    int m_cur_alt;


public:
    NodeCursor(NodeID start, const NodeTree& tree);

/// Test if the cursor may move to the parent node
    bool mayMoveUpwards() const;
/// Move cursor to the parent node
    void moveUpwards();
/// Test if cursor may move to the first child node
    bool mayMoveDownwards() const;
/// Move cursor to the first child node
    void moveDownwards();
/// Test if cursor may move to the first sibling
    bool mayMoveSidewards() const;
/// Move cursor to the first sibling
    void moveSidewards();

    void finalize();

    void processCurrentNode();
};

class UnsafeNodeCursor {

protected:

    const Structure& m_tree;
    const NodeInfo& m_node_info;
    const NodeID m_start_node;
    NodeID m_cur_node;
    int m_cur_alt;


public:
    UnsafeNodeCursor(NodeID start, const NodeTree& tree);

/// Test if the cursor may move to the parent node
    bool mayMoveUpwards() const;
/// Move cursor to the parent node
    void moveUpwards();
/// Test if cursor may move to the first child node
    bool mayMoveDownwards() const;
/// Move cursor to the first child node
    void moveDownwards();
/// Test if cursor may move to the first sibling
    bool mayMoveSidewards() const;
/// Move cursor to the first sibling
    void moveSidewards();

    void finalize();

    void processCurrentNode();
};


}}


#endif
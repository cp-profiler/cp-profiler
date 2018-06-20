#ifndef CPPROFILER_TREE_CURSOR_NODE_CURSOR_HH
#define CPPROFILER_TREE_CURSOR_NODE_CURSOR_HH

#include "../node_id.hh"

namespace cpprofiler
{
namespace tree
{

class Structure;
class NodeInfo;
class NodeTree;

class NodeCursor
{

  private:
    NodeID node_;
    int cur_alt_;
    const NodeID start_node_;

  protected:
    const NodeTree &tree_;

    /// return current node
    inline NodeID cur_node() const { return node_; }
    inline int cur_alt() const { return cur_alt_; }
    inline NodeID start_node() const { return start_node_; }

  public:
    NodeCursor(NodeID start, const NodeTree &tree);

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

} // namespace tree
} // namespace cpprofiler

#endif
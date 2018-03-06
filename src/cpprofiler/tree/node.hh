#ifndef CPPROFILER_TREE_NODE
#define CPPROFILER_TREE_NODE

#include <vector>
#include "../core.hh"

namespace cpprofiler { namespace tree {

struct no_child : std::exception {
    const char* what() const throw() override {
        return "child does not exist at alt\n";
    }
};

enum class NodeStatus  {
    SOLVED = 0,
    FAILED = 1,
    BRANCH = 2,
    SKIPPED = 3,
    UNDETERMINED = 4
};

QDebug&& operator<<(QDebug&& out, NodeStatus status);

class Node {
private:
    enum class Tag {
        LEAF = 0,
        ONE_CHILD,
        TWO_CHILDREN,
        MORE_CHILDREN
    };

    /// The parent of this node
    NodeID m_parent;

    /// The children, or in case there are at most two, the first child
    void* m_childrenOrFirstChild;

    NodeID* m_children;

    // The number of children, in case it is greater than 2, or the second child (if negative)
    int m_noOfChildren;

    Tag getTag() const;
    void setTag(Tag);

    void setParent(NodeID pid);

public:

    Node(NodeID parent_nid, int kids = 0);

    int getNumberOfChildren() const;
    void setNumberOfChildren(int n);

    /// similar to setNumberOfChildren, but handles existing children
    void resetNumberOfChildren(int n);

    void setChild(NodeID, int alt);
    NodeID getChild(int alt);

    NodeID getParent() const;

    ~Node();
    Node& operator=(const Node&) = delete;
    Node(const Node&) = delete;

};


}}

#endif
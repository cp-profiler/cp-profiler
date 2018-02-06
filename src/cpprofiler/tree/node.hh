#ifndef CPPROFILER_TREE_NODE
#define CPPROFILER_TREE_NODE

#include <vector>

namespace cpprofiler { namespace tree {

struct NodeID {
    int id;
};

class Node {
private:
    enum {
        TWO_CHILDREN,
        MORE_CHILDREN
    };

    /// The parent of this node
    NodeID parent;

    /// The children, or in case there are at most two, the first child
    void* childrenOrFirstChild;

    // The number of children, in case it is greater than 2, or the first child (if negative)
    int noOfChildren;


};


}}

#endif
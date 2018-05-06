#ifndef CPPROFILER_NODE_TREE_HH
#define CPPROFILER_NODE_TREE_HH

#include "node.hh"

#include "../core.hh"

#include "memory"


namespace cpprofiler { namespace tree {

struct invalid_tree : std::exception {
    const char* what() const throw() override {
        return "invalid tree\n";
    }
};

class LayoutComputer;

class Structure {

    mutable utils::Mutex m_structure_mutex;

    std::vector<std::unique_ptr<Node>> m_nodes;

    /// Allocate memory for a new node and return its Id
    NodeID createNode(NodeID pid, int kids);

    /// Create a new node with `kids` kids and add set it as the `alt`th kid of `pid`
    NodeID createChild(NodeID pid, int alt, int kids);

public:

    Structure();

    utils::Mutex& getMutex() const;

    NodeID getParent(NodeID nid) const;
    NodeID getParent_safe(NodeID nid) const;

    NodeID getChild(NodeID pid, int alt) const;

    /// which alternative the current node is relative to its parent
    int getAlternative(NodeID nid) const;
    int getAlternative_safe(NodeID nid) const;

    int getNumberOfSiblings(NodeID nid) const;

    int childrenCount(NodeID pid) const;

    NodeID getRoot() const;
    int nodeCount() const;

    int calculateDepth(NodeID nid) const;

    int nodeCount_safe() const;

    /// Create a root node and `kids` children
    NodeID createRoot(int kids);

    /// Extend node's children with one more child
    NodeID addExtraChild(NodeID nid);

    /// Add `kids` children to an open node
    void addChildren(NodeID nid, int kids);

};

}}


#endif
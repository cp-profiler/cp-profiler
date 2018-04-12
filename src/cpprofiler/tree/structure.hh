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

    friend class LayoutComputer;
    friend class TraditionalView;

    // mutable QReadWriteLock m_lock;
    mutable utils::Mutex m_structure_mutex;

    std::vector<std::unique_ptr<Node>> m_nodes;

public:

    Structure();

    utils::Mutex& getMutex() const;

    NodeID getParent(NodeID nid) const;
    NodeID getParent_safe(NodeID nid) const;

    NodeID getChild(NodeID pid, int alt) const;
    NodeID getChild_safe(NodeID pid, int alt) const;

    /// which alternative the current node is relative to its parent
    int getAlternative(NodeID nid) const;
    int getAlternative_safe(NodeID nid) const;

    int getNumberOfSiblings(NodeID nid) const;

    int childrenCount(NodeID pid) const;

    NodeID createRoot(int kids);

    /// Create a new node with `kids` kids and add set is as a `alt`th kid of `pid`
    NodeID addChild(NodeID pid, int alt, int kids);

    NodeID getRoot() const;

    int nodeCount() const;

    NodeID addChild_safe(NodeID pid, int alt, int kids);

    /// Reset node count and create undet nodes for children
    void resetNumberOfChildren(NodeID nid, int kids);
    void resetNumberOfChildren_safe(NodeID nid, int kids);

    int calculateDepth(NodeID nid) const;
    int calculateDepth_safe(NodeID nid) const;

    int nodeCount_safe() const;
};

}}


namespace cpprofiler { namespace tree { namespace helper {

std::vector<NodeID> postOrder(const Structure& tree);

std::vector<NodeID> preOrder(const Structure& tree);

std::vector<NodeID> anyOrder(const Structure& tree);


}}}


#endif
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

    /// Same as `getParent` without holding a mutex
    NodeID getParent_unsafe(NodeID nid) const;

    int childrenCount_unsafe(NodeID pid) const;

    NodeID addChild_unsafe(NodeID pid, int alt, int kids);

    NodeID getChild_unsafe(NodeID pid, int alt) const;

    NodeID getRoot_unsafe() const;

    int nodeCount_unsafe() const;

    utils::Mutex& getMutex() const;

    Structure();

    NodeID createRoot(int kids);

    NodeID createRoot_unsafe(int kids);

    NodeID addChild(NodeID pid, int alt, int kids);

    void resetNumberOfChildren(NodeID nid, int kids);

    void resetNumberOfChildren_unsafe(NodeID nid, int kids);

    NodeID getChild(NodeID pid, int alt) const;

    NodeID getParent(NodeID nid) const;

    int childrenCount(NodeID pid) const;

    int getNumberOfSiblings(NodeID nid) const;
    int getNumberOfSiblings_unsafe(NodeID nid) const;

    NodeID getRoot() const;

    /// which alternative the current node is relative to its parent
    int getAlternative(NodeID nid) const;
    int getAlternative_unsafe(NodeID nid) const;

    int calculateDepth(NodeID nid) const;
    int calculateDepth_unsafe(NodeID nid) const;

    int nodeCount() const;

};

}}


namespace cpprofiler { namespace tree { namespace helper {

std::vector<NodeID> postOrder(const Structure& tree);

std::vector<NodeID> postOrder_unsafe(const Structure& tree);

std::vector<NodeID> preOrder(const Structure& tree);

std::vector<NodeID> anyOrder(const Structure& tree);


}}}


#endif
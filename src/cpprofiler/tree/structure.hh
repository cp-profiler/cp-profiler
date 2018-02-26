#ifndef CPPROFILER_NODE_TREE_HH
#define CPPROFILER_NODE_TREE_HH

#include "node.hh"
#include "memory"

#include <QMutex>

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
    mutable QMutex m_structure_mutex;

    std::vector<std::unique_ptr<Node>> m_nodes;

public:

    /// Same as `getParent` without holding a mutex
    NodeID getParent_unsafe(NodeID nid) const;

    int getNumberOfChildren_unsafe(NodeID pid) const;

    NodeID addChild_unsafe(NodeID pid, int alt, int kids);

    NodeID getChild_unsafe(NodeID pid, int alt) const;

    NodeID getRoot_unsafe() const;

    int nodeCount_unsafe() const;

    QMutex& getMutex() const;

    Structure();

    NodeID createRoot(int kids);

    NodeID addChild(NodeID pid, int alt, int kids);

    void resetNumberOfChildren(NodeID nid, int kids);

    NodeID getChild(NodeID pid, int alt) const;

    NodeID getParent(NodeID nid) const;

    int getNumberOfChildren(NodeID pid) const;

    int getNumberOfSiblings(NodeID nid) const;

    NodeID getRoot() const;

    /// which alternative the current node is relative to its parent
    int getAlternative(NodeID nid) const;

    int nodeCount() const;

};

}}


namespace cpprofiler { namespace tree { namespace helper {

std::vector<NodeID> postOrder(const Structure& tree);

std::vector<NodeID> postOrder_unsafe(const Structure& tree);

std::vector<NodeID> preOrder(const Structure& tree);


}}}


#endif
#ifndef CPPROFILER_NODE_TREE_HH
#define CPPROFILER_NODE_TREE_HH

#include "node.hh"
#include "memory"

#include <QReadWriteLock>

namespace cpprofiler { namespace tree {

struct invalid_tree : std::exception {
    const char* what() const throw() override {
        return "invalid tree\n";
    }
};

class Structure {

    mutable QReadWriteLock m_lock;

    std::vector<std::unique_ptr<Node>> m_nodes;

    /// Same as `getParent` without holding a mutex
    NodeID getParent_unsafe(NodeID nid) const;

    int getNumberOfChildren_unsafe(NodeID pid) const;

    NodeID getChild_unsafe(NodeID pid, int alt) const;

    NodeID getRoot_unsafe() const;


public:

    Structure();

    NodeID createRoot(int kids);

    NodeID addChild(NodeID pid, int alt, int kids);

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

std::vector<NodeID> preOrder(const Structure& tree);


}}}


#endif
#ifndef CPPROFILER_NODE_TREE_HH
#define CPPROFILER_NODE_TREE_HH

#include "node.hh"
#include "memory"

namespace cpprofiler { namespace tree {

struct invalid_tree : std::exception {
    const char* what() const throw() override {
        return "invalid tree\n";
    }
};

class Structure {

    std::vector<std::unique_ptr<Node>> m_nodes;


public:

    Structure();

    NodeID createRoot(int kids);

    NodeID addChild(NodeID pid, int alt, int kids);

    NodeID getChild(NodeID pid, int alt) const;

    int getNumberOfChildren(NodeID pid) const;

    int nodeCount() const;

};

}}


namespace cpprofiler { namespace tree { namespace helper {

std::vector<NodeID> postOrder(const Structure& tree);

std::vector<NodeID> preOrder(const Structure& tree);


}}}


#endif
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

    void createRoot();

};

}}


#endif
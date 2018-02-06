
#include "execution.hh"
#include "tree/node_tree.hh"

#include <iostream>

namespace cpprofiler {

    std::string Execution::name() {
        return m_name;
    }

    Execution::Execution() : m_tree{new tree::NodeTree()} {
        std::cerr << "Execution()\n";
    }

    Execution::~Execution() {
        std::cerr << "~Execution()\n";
    }

    const tree::Structure& Execution::tree_structure() const {
        return m_tree->tree_structure();
    }

    tree::Structure& Execution::tree_structure() {
        return m_tree->tree_structure();
    }
}
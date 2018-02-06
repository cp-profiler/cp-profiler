#include "node_tree.hh"

#include "structure.hh"
#include "layout.hh"

namespace cpprofiler { namespace tree {

NodeTree::NodeTree() : m_structure{new Structure()} {}

NodeTree::~NodeTree() = default;

const tree::Structure& NodeTree::tree_structure() const {
    return *m_structure;
}

tree::Structure& NodeTree::tree_structure() {
    return *m_structure;
}


    
}}
#ifndef CPPROFILER_TREE_NODE_TREE_HH
#define CPPROFILER_TREE_NODE_TREE_HH

#include <memory>

namespace cpprofiler { namespace tree {

class Structure;
class Layout;

class NodeTree {

    std::unique_ptr<Structure> m_structure;
    std::unique_ptr<Layout> m_layout;

public:

    NodeTree();
    ~NodeTree();

    tree::Structure& tree_structure();
    const tree::Structure& tree_structure() const;


};

}}




#endif
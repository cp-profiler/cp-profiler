#ifndef CPPROFILER_EXECUTION_HH
#define CPPROFILER_EXECUTION_HH

#include <string>
#include <memory>
#include "tree/node_tree.hh"

namespace cpprofiler {

class Execution {
    std::string m_name;

    std::unique_ptr<tree::NodeTree> m_tree;

public:
    std::string name();

    Execution();
    ~Execution();

    tree::Structure& tree_structure();
    const tree::Structure& tree_structure() const;
};

}



#endif


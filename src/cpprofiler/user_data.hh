#ifndef CPPROFILER_USER_DATA_HH
#define CPPROFILER_USER_DATA_HH

#include "tree/node_id.hh"

namespace cpprofiler {

class UserData {

    tree::NodeID m_selected_node = tree::NodeID::NoNode;

public:

    void setSelectedNode(tree::NodeID nid);

    /// returns currently selected node (possibly NodeID::NoNode)
    tree::NodeID getSelectedNode() const;

};

}

#endif
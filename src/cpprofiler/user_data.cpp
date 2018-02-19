#include "user_data.hh"

namespace cpprofiler {

    void UserData::setSelectedNode(tree::NodeID nid) {
        m_selected_node = nid;
    }

    tree::NodeID UserData::getSelectedNode() const {
        return m_selected_node;
    }


}
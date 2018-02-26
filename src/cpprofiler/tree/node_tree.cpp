#include "node_tree.hh"

#include "structure.hh"
#include "node_info.hh"
#include <QDebug>

namespace cpprofiler { namespace tree {

NodeTree::NodeTree() : m_structure{new Structure()} {
    m_node_info.reset(new NodeInfo);
}

NodeTree::~NodeTree() = default;

const Structure& NodeTree::tree_structure() const {
    return *m_structure;
}

Structure& NodeTree::tree_structure() {
    return *m_structure;
}

const NodeInfo& NodeTree::node_info() const {
    return *m_node_info;
}

NodeInfo& NodeTree::node_info() {
    return *m_node_info;
}

NodeID NodeTree::addNode(NodeID parent_id, int alt, int kids, tree::NodeStatus status) {

    auto nodes_created = 0;

    NodeID nid;
    if (parent_id == tree::NodeID::NoNode) {
        nid = m_structure->createRoot(kids);
        m_node_info->addEntry(nid);
        nodes_created += (1 + kids);
    } else {
        nid = m_structure->getChild(parent_id, alt);

        if (kids > 0) {
            m_structure->resetNumberOfChildren(nid, kids);
            nodes_created += (kids - 1);
        }
    }

    for (auto i = 0; i < kids; ++i) {
        auto child_nid = m_structure->getChild(nid, i);
        m_node_info->addEntry(child_nid);
        m_node_info->setStatus(child_nid, NodeStatus::UNDETERMINED);
    }

    m_node_info->setStatus(nid, status);

    emit structureUpdated();
    emit nodesCreated(nodes_created);

    return nid;
}

int NodeTree::nodeCount() const {
    return m_structure->nodeCount();
}



}}
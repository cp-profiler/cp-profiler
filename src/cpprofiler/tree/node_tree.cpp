#include "node_tree.hh"

#include "structure.hh"
#include "node_info.hh"
#include <QDebug>

namespace cpprofiler { namespace tree {

NodeTree::NodeTree() : m_structure{new Structure()} {
    m_node_info.reset(new NodeInfo);

    connect(&m_node_stats, &NodeStats::stats_changed, this, &NodeTree::node_stats_changed);
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
    if (parent_id == NodeID::NoNode) {

        nid = m_structure->createRoot(kids);
        m_node_info->addEntry(nid);
        nodes_created += (1 + kids);

    } else {
        nid = m_structure->getChild(parent_id, alt);

        if (kids > 0) {
            m_structure->resetNumberOfChildren(nid, kids);
            nodes_created += (kids - 1);
        }

        emit childrenStructureChanged(parent_id);
    }

    for (auto i = 0; i < kids; ++i) {
        auto child_nid = m_structure->getChild(nid, i);
        m_node_info->addEntry(child_nid);
        m_node_info->setStatus(child_nid, NodeStatus::UNDETERMINED);

        m_node_stats.add_undetermined(kids);
    }

    m_node_info->setStatus(nid, status);

    switch (status) {
        case NodeStatus::BRANCH: {
            m_node_stats.subtract_undetermined(1);
            m_node_stats.add_branch(1);
        } break;
        case NodeStatus::FAILED: {
            m_node_stats.subtract_undetermined(1);
            m_node_stats.add_failed(1);
        } break;
        case NodeStatus::SOLVED: {
            m_node_stats.subtract_undetermined(1);
            m_node_stats.add_solved(1);
        } break;
        case NodeStatus::UNDETERMINED: {
            // do nothing
        } break;
    }

    auto cur_depth = m_structure->calculateDepth(nid);
    m_node_stats.inform_depth(cur_depth);

    emit structureUpdated();
    emit nodesCreated(nodes_created);

    return nid;
}

int NodeTree::nodeCount() const {
    return m_structure->nodeCount();
}

NodeID NodeTree::getParent(NodeID nid) const {
    return m_structure->getParent(nid);
}

NodeID NodeTree::getChild(NodeID nid, int alt) const {
    return m_structure->getChild(nid, alt);
}

const NodeStats& NodeTree::node_stats() const {
    return m_node_stats;
}

NodeStatus NodeTree::status(NodeID nid) const {
    m_node_info->getStatus(nid);
}

int NodeTree::depth() const {
    m_node_stats.max_depth();
}

int NodeTree::calculateDepth(NodeID nid) const {
    return m_structure->calculateDepth(nid);
}

int NodeTree::getAlternative(NodeID nid) const {
    return m_structure->getAlternative(nid);
}

int NodeTree::getNumberOfChildren(NodeID nid) const {
    return m_structure->getNumberOfChildren(nid);
}



}}
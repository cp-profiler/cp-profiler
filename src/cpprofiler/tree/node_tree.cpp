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

void NodeTree::addEntry(NodeID nid) {
    m_node_info->addEntry(nid);
    m_labels.push_back({});
}

const Structure& NodeTree::tree_structure() const {
    return *m_structure;
}

Structure& NodeTree::tree_structure() {
    return *m_structure;
}

NodeID NodeTree::addChild_safe(NodeID pid, int alt, int kids) {
    return m_structure->addChild_safe(pid, alt, kids);
}

const NodeInfo& NodeTree::node_info() const {
    return *m_node_info;
}

NodeInfo& NodeTree::node_info() {
    return *m_node_info;
}

NodeID NodeTree::createRoot_safe(int kids) {
    return m_structure->createRoot_safe(kids);
}

NodeID NodeTree::createRoot(int kids) {
    return m_structure->createRoot(kids);
}

NodeID NodeTree::addNode(NodeID parent_id, int alt, int kids, tree::NodeStatus status, Label label) {

    auto nodes_created = 0;

    NodeID nid;
    if (parent_id == NodeID::NoNode) {

        nid = m_structure->createRoot_safe(kids);
        addEntry(nid);
        setLabel(nid, label);
        nodes_created += (1 + kids);

        m_node_stats.add_undetermined(1);

    } else {
        nid = m_structure->getChild_safe(parent_id, alt);

        if (kids > 0) {
            m_structure->resetNumberOfChildren_safe(nid, kids);
            nodes_created += (kids - 1);
        }

        setLabel(nid, label);

        emit childrenStructureChanged(parent_id);
    }

    for (auto i = 0; i < kids; ++i) {
        auto child_nid = m_structure->getChild_safe(nid, i);
        addEntry(child_nid);
        m_node_info->setStatus(child_nid, NodeStatus::UNDETERMINED);
    }

    m_node_stats.add_undetermined(kids);

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

    auto cur_depth = m_structure->calculateDepth_safe(nid);
    m_node_stats.inform_depth(cur_depth);

    emit structureUpdated();
    emit nodesCreated(nodes_created);

    return nid;
}

int NodeTree::nodeCount_safe() const {
    return m_structure->nodeCount_safe();
}

NodeID NodeTree::getParent_safe(NodeID nid) const {
    return m_structure->getParent_safe(nid);
}

NodeID NodeTree::getRoot_safe() const {
    return m_structure->getRoot_safe();
}

NodeID NodeTree::getRoot() const {
    return m_structure->getRoot();
}

NodeID NodeTree::getChild_safe(NodeID nid, int alt) const {
    return m_structure->getChild_safe(nid, alt);
}

NodeID NodeTree::getChild(NodeID nid, int alt) const {
    return m_structure->getChild(nid, alt);
}

const NodeStats& NodeTree::node_stats() const {
    return m_node_stats;
}

utils::Mutex& NodeTree::treeMutex() const {
    return m_structure->getMutex();
}

NodeStatus NodeTree::status(NodeID nid) const {
    m_node_info->getStatus(nid);
}

int NodeTree::getNumberOfSiblings_safe(NodeID nid) const {
    return m_structure->getNumberOfSiblings_safe(nid);
}

int NodeTree::depth() const {
    m_node_stats.max_depth();
}

int NodeTree::calculateDepth_safe(NodeID nid) const {
    return m_structure->calculateDepth_safe(nid);
}

int NodeTree::getAlternative_safe(NodeID nid) const {
    return m_structure->getAlternative_safe(nid);
}

bool NodeTree::isRightMostChild(NodeID nid) const {
    // QMutexLocker lock(&m_structure->getMutex());
    auto pid = m_structure->getParent(nid);

    /// root is treated as the left-most child
    if (pid == NodeID::NoNode) return false;

    auto kids = m_structure->childrenCount(pid);
    auto alt = m_structure->getAlternative(nid);
    if (alt == kids-1) { return true; } else { return false; }
}

bool NodeTree::isLeaf_safe(NodeID nid) const {
    return m_structure->childrenCount_safe(nid) == 0;
}

int NodeTree::childrenCount_safe(NodeID nid) const {
    return m_structure->childrenCount_safe(nid);
}

int NodeTree::childrenCount(NodeID nid) const {
    return m_structure->childrenCount(nid);
}

void NodeTree::resetNumberOfChildren(NodeID nid, int kids) {
    m_structure->resetNumberOfChildren(nid, kids);
}

const Label& NodeTree::getLabel(NodeID nid) const {
    return m_labels.at(nid);
}

void NodeTree::setLabel(NodeID nid, const Label& label) {
    m_labels[nid] = label;
}



}}
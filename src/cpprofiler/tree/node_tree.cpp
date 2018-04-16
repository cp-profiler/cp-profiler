#include "node_tree.hh"

#include "structure.hh"
#include "node_info.hh"
#include <QDebug>
#include <cassert>

namespace cpprofiler { namespace tree {


NodeTree::NodeTree() : m_structure{new Structure()}, m_node_info(new NodeInfo) {
    // connect(&m_node_stats, &NodeStats::stats_changed, this, &NodeTree::node_stats_changed);
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

const NodeInfo& NodeTree::node_info() const {
    return *m_node_info;
}

NodeInfo& NodeTree::node_info() {
    return *m_node_info;
}

NodeID NodeTree::createRoot_safe(int kids) {
    utils::MutexLocker locker(&treeMutex(), "create root");
    return m_structure->createRoot(kids);
}

void NodeTree::promoteNode(NodeID nid, int kids, NodeStatus status, Label label) {

    /// find parent and kid's position
    const auto pid = getParent(nid);
    const auto alt = getAlternative(nid);

    promoteNode(pid, alt, kids, status, label);
}

/// create undet root that can be later transformed into a branch node
NodeID NodeTree::createDummyRoot() {
    auto nid = m_structure->createRoot(0);
    addEntry(nid);

    m_node_stats.inform_depth(1);
    m_node_stats.add_undetermined(1);

    m_node_info->setStatus(nid, NodeStatus::UNDETERMINED);

    emit structureUpdated();
}


NodeID NodeTree::createRoot(int kids, Label label) {
    auto nid = m_structure->createRoot(kids);
    addEntry(nid);
    setLabel(nid, label);

    auto depth = kids > 0 ? 2 : 1;

    m_node_stats.inform_depth(depth);
    m_node_stats.add_branch(1);

    m_node_info->setStatus(nid, NodeStatus::BRANCH);

    for (auto i = 0; i < kids; ++i) {
        auto child_nid = m_structure->getChild(nid, i);
        addEntry(child_nid);
        m_node_info->setStatus(child_nid, NodeStatus::UNDETERMINED);
    }

    m_node_stats.add_undetermined(kids);

    emit structureUpdated();

    return nid;
}

void NodeTree::addExtraChild(NodeID pid) {
    const auto nid = m_structure->addExtraChild(pid);
    addEntry(nid);

    m_node_info->setStatus(nid, NodeStatus::UNDETERMINED);
    m_node_stats.add_undetermined(1);

    emit structureUpdated();
}

NodeID NodeTree::promoteNode(NodeID parent_id, int alt, int kids, tree::NodeStatus status, Label label) {

    NodeID nid;
    
    if (parent_id == NodeID::NoNode) {
        /// This makes it possible to transform an undet root node into a branch node,
        /// necessary, for example, in merging
        nid = m_structure->getRoot();
    } else {
        nid = m_structure->getChild(parent_id, alt);
    }

    m_node_info->setStatus(nid, status);
    setLabel(nid, label);

    if (kids > 0) {
        m_structure->addChildren(nid, kids);
        emit childrenStructureChanged(parent_id); /// updates dirty status for nodes

        for (auto i = 0; i < kids; ++i) {
            auto child_nid = m_structure->getChild(nid, i);
            addEntry(child_nid);
            m_node_info->setStatus(child_nid, NodeStatus::UNDETERMINED);
        }

        m_node_stats.add_undetermined(kids);

        auto cur_depth = m_structure->calculateDepth(nid);
        m_node_stats.inform_depth(cur_depth + 1);
    }

    switch (status) {
        case NodeStatus::BRANCH: {
            m_node_stats.subtract_undetermined(1);
            m_node_stats.add_branch(1);
        } break;
        case NodeStatus::FAILED: {
            m_node_stats.subtract_undetermined(1);
            m_node_stats.add_failed(1);
            closeNode(nid);
        } break;
        case NodeStatus::SOLVED: {
            m_node_stats.subtract_undetermined(1);
            m_node_stats.add_solved(1);
            notifyAncestors(nid);
            closeNode(nid);
        } break;
        case NodeStatus::SKIPPED: {
            closeNode(nid);
        }
        case NodeStatus::UNDETERMINED: {
            // do nothing
        } break;
    }

    emit structureUpdated();

    return nid;
}

int NodeTree::nodeCount_safe() const {
    return m_structure->nodeCount_safe();
}

int NodeTree::nodeCount() const {
    return m_structure->nodeCount();
}

NodeID NodeTree::getParent_safe(NodeID nid) const {
    return m_structure->getParent_safe(nid);
}

NodeID NodeTree::getParent(NodeID nid) const {
    return m_structure->getParent(nid);
}

NodeID NodeTree::getRoot_safe() const {
    utils::MutexLocker locker(&treeMutex(), "get root");
    return m_structure->getRoot();
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

NodeStatus NodeTree::getStatus(NodeID nid) const {
    m_node_info->getStatus(nid);
}

int NodeTree::getNumberOfSiblings(NodeID nid) const {
    return m_structure->getNumberOfSiblings(nid);
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

int NodeTree::getAlternative(NodeID nid) const {
    return m_structure->getAlternative(nid);
}

bool NodeTree::isRightMostChild(NodeID nid) const {
    auto pid = m_structure->getParent(nid);

    /// root is treated as the left-most child
    if (pid == NodeID::NoNode) return false;

    auto kids = m_structure->childrenCount(pid);
    auto alt = m_structure->getAlternative(nid);
    if (alt == kids-1) { return true; } else { return false; }
}

bool NodeTree::isLeaf(NodeID nid) const {
    return m_structure->childrenCount(nid) == 0;
}

int NodeTree::childrenCount(NodeID nid) const {
    return m_structure->childrenCount(nid);
}

void NodeTree::notifyAncestors(NodeID nid) {
    while(nid != NodeID::NoNode) {
        m_node_info->setHasSolvedChildren(nid, true);
        nid = getParent(nid);
    }
}

void NodeTree::setHasOpenChildren(NodeID nid, bool val) {
    m_node_info->setHasOpenChildren(nid, val);
}

const Label& NodeTree::getLabel(NodeID nid) const {
    return m_labels.at(nid);
}

bool NodeTree::hasSolvedChildren(NodeID nid) const {
    return m_node_info->hasSolvedChildren(nid);
}

bool NodeTree::hasOpenChildren(NodeID nid) const {
    return m_node_info->hasOpenChildren(nid);
}

bool NodeTree::isOpen(NodeID nid) const {
    return ((getStatus(nid) == NodeStatus::UNDETERMINED) ||
            hasOpenChildren(nid));
}

void NodeTree::onChildClosed(NodeID nid) {

    bool allClosed = true;

    for (auto i = childrenCount(nid); i--;) {
        auto kid = getChild(nid, i);
        if (isOpen(kid)) {
            allClosed = false;
            break;
        }
    }

    if (allClosed) { closeNode(nid); }

}

void NodeTree::closeNode(NodeID nid) {
    setHasOpenChildren(nid, false);
    auto pid = getParent(nid);
    if (pid != NodeID::NoNode) {
        onChildClosed(pid);
    }
}

void NodeTree::setLabel(NodeID nid, const Label& label) {
    m_labels[nid] = label;
}

void NodeTree::setStatus(NodeID nid, NodeStatus status) {
    m_node_info->setStatus(nid, status);
}



}}
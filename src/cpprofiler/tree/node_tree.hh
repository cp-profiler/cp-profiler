#ifndef CPPROFILER_TREE_NODE_TREE_HH
#define CPPROFILER_TREE_NODE_TREE_HH

#include <QObject>
#include <memory>
#include <string>
#include <stack>
#include "node_id.hh"
#include "node.hh"
#include "../core.hh"

#include "node_stats.hh"

namespace cpprofiler {
    class NameMap;
}

namespace cpprofiler { namespace tree {

class Structure;
class NodeInfo;

using Label = std::string;
static Label emptyLabel = {};

/// Node tree encapsulates tree structure, node statistics (number of nodes etc.),
/// status for nodes (m_node_info), labels
class NodeTree : public QObject {
Q_OBJECT
    std::unique_ptr<Structure> m_structure;

    std::unique_ptr<NodeInfo> m_node_info;

    std::shared_ptr<const NameMap> name_map_;

    std::vector<Label> m_labels;

    NodeStats m_node_stats;

    /// Ensure all relevant data structures contain this node
    void addEntry(NodeID nid);

    /// notify ancestor nodes of a solution
    void notifyAncestors(NodeID nid);

    /// notify ancestor nodes that of whether they contain open nodes
    void onChildClosed(NodeID nid);

    /// set closed and notify ancestors
    void closeNode(NodeID nid);

public:

    NodeTree();
    ~NodeTree();

    const NodeInfo& node_info() const;
    NodeInfo& node_info();

    const NodeStats& node_stats() const;

    cpprofiler::utils::Mutex& treeMutex() const;

    void setNameMap(std::shared_ptr<const NameMap> nm);

    /// *************************** Tree Modifiers ***************************

    NodeID createRoot(int kids, Label label = emptyLabel);

    NodeID createRoot_safe(int kids);

    NodeID createDummyRoot();

    /// turn a white node into some other node
    void promoteNode(NodeID nid, int kids, NodeStatus status, Label = emptyLabel);

    /// TODO: rename it to resetNode
    /// Turn undet node into a real one, updating stats and emitting signals
    NodeID promoteNode(NodeID parent_id, int alt, int kids, NodeStatus status, Label = emptyLabel);

    void addExtraChild(NodeID pid);

    /// Set the flag for open children
    void setHasOpenChildren(NodeID nid, bool val);

    void setLabel(NodeID nid, const Label& label);

    void setStatus(NodeID nid, NodeStatus status);

    /// ********************************************************************
    /// *************************** Tree Queries ***************************

    /// return the depth of the tree
    int depth() const;

    NodeID getRoot_safe() const;
    NodeID getRoot() const;

    /// Get the total nuber of nodes (including undetermined)
    int nodeCount() const;

    /// Get the total nuber of siblings of `nid` including the node itself
    int getNumberOfSiblings(NodeID nid) const;

    /// Get the position of node `nid` relative to its left-most sibling
    int getAlternative(NodeID nid) const;

    /// Get the total number of children of node `pid`
    int childrenCount(NodeID nid) const;

    /// Get the child of node `pid` at position `alt`
    NodeID getChild(NodeID nid, int alt) const;

    /// Get the parent of node `nid` (returns NodeID::NoNode for root)
    NodeID getParent(NodeID nid) const;

    /// Get the status of node `nid`
    NodeStatus getStatus(NodeID nid) const;

    /// Get the label of node `nid`
    const Label getLabel(NodeID nid) const;

    /// Inquire if the node `nid` has solved children (ancestors?)
    bool hasSolvedChildren(NodeID nid) const;

    /// Inquire if the node `nid` has open children (ancestors?)
    bool hasOpenChildren(NodeID nid) const;

    /// Inquire if the node `nid` is open or has open children
    bool isOpen(NodeID nid) const;

    /// ********************************************************************

signals:

    void structureUpdated();

    /// sets immediate children and nodes up the tree as dirty
    void childrenStructureChanged(NodeID nid);

    void node_stats_changed();

};

}}




#endif
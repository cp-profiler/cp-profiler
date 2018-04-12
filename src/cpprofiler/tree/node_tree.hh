#ifndef CPPROFILER_TREE_NODE_TREE_HH
#define CPPROFILER_TREE_NODE_TREE_HH

#include <QObject>
#include <memory>
#include <string>
#include <stack>
#include "node_id.hh"
#include "node.hh"

namespace cpprofiler { namespace tree {

class Structure;
class NodeInfo;

class NodeStats : public QObject {
Q_OBJECT
    int m_branch = 0;
    int m_undetermined = 0;
    int m_failed = 0;
    int m_solved = 0;
    int m_max_depth = 0;

public:

    int branch_count() const {
        return m_branch;
    }

    int undetermined_count() const {
        return m_undetermined;
    }

    int failed_count() const {
        return m_failed;
    }

    int solved_count() const {
        return m_solved;
    }

    int max_depth() const {
        return m_max_depth;
    }

    void add_branch(int n) {
        m_branch = m_branch + n;
        emit stats_changed();
    }

    void subtract_undetermined(int n) {
        m_undetermined = m_undetermined - n;
        emit stats_changed();
    }

    void add_undetermined(int n) {
        m_undetermined = m_undetermined + n;
        emit stats_changed();
    }

    void add_failed(int n) {
        m_failed = m_failed + n;
        emit stats_changed();
    }

    void add_solved(int n) {
        m_solved = m_solved + n;
        emit stats_changed();
    }

    /// see if max depth needs to be updated to d
    void inform_depth(int d) {
        if (d > m_max_depth) {
            m_max_depth = d;
            emit stats_changed();
        }
    }

signals:

    void stats_changed();


};

using Label = std::string;

static Label emptyLabel = {};


/// Node tree encapsulates tree structure,
/// node statistics (number of nodes etc.),
/// status for nodes (m_node_info), labels
class NodeTree : public QObject {
Q_OBJECT
    std::unique_ptr<Structure> m_structure;


    /// How do I make sure that this is syncronised with the number of nodes?
    /// (using NodeTree::addEntry method)

    /// NodeTree::addNode calls addEntry for every node (including white nodes)

    /// NodeTree::resetNumberOfChildren creates new nodes,
    /// but doesn't update the data structures
    std::unique_ptr<NodeInfo> m_node_info;

    std::vector<Label> m_labels;

    NodeStats m_node_stats;

    /// Ensure all relevant data structures contain this node
    void addEntry(NodeID nid);

    /// Not sure if I need to keep this
    void resetNumberOfChildren(NodeID nid, int kids);

    /// notify ancestor nodes of a solution
    void notifyAncestors(NodeID nid);

    /// notify ancestor nodes that of whether they contain open nodes
    void onChildClosed(NodeID nid);

    /// set closed and notify ancestors
    void closeNode(NodeID nid);

public:

    NodeTree();
    ~NodeTree();

    const tree::Structure& tree_structure() const;
    tree::Structure& tree_structure();

    const NodeInfo& node_info() const;
    NodeInfo& node_info();

    const NodeStats& node_stats() const;

    utils::Mutex& treeMutex() const;

    /// *************************** Tree Modifiers ***************************

    NodeID createRoot(int kids, Label label = emptyLabel);

    NodeID createRoot_safe(int kids);

    NodeID createDummyRoot();

    NodeID addChild(NodeID pid, int alt, int kids);

    /// turn a white node into some other node
    void transformNode(NodeID nid, int kids, NodeStatus status, Label = emptyLabel);

    // NodeID addNode(NodeID parent_id, int alt, int kids, NodeStatus status, Label = emptyLabel);

    /// Turn undet node into a real one, updating stats and emitting signals
    NodeID addNodeNew(NodeID parent_id, int alt, int kids, NodeStatus status, Label = emptyLabel);

    /// Set the flage for open children
    void setHasOpenChildren(NodeID nid, bool val);

    void setLabel(NodeID nid, const Label& label);

    void setStatus(NodeID nid, NodeStatus status);

    /// ********************************************************************
    /// *************************** Tree Queries ***************************

    /// return the depth of the tree
    int depth() const;

    NodeID getRoot_safe() const;
    NodeID getRoot() const;

    /// Total number of nodes (including undetermined)
    int nodeCount_safe() const;

    int nodeCount() const;

    NodeStatus getStatus(NodeID nid) const;

    int getNumberOfSiblings(NodeID nid) const;

    int getAlternative_safe(NodeID nid) const;
    int getAlternative(NodeID nid) const;

    int childrenCount(NodeID nid) const;

    NodeID getChild_safe(NodeID nid, int alt) const;
    NodeID getChild(NodeID nid, int alt) const;

    NodeID getParent(NodeID nid) const;
    NodeID getParent_safe(NodeID nid) const;

    /// whether the node is the right-most child
    bool isRightMostChild(NodeID nid) const;

    bool isLeaf(NodeID) const;

    /// return the depth of the node
    int calculateDepth_safe(NodeID nid) const;

    const Label& getLabel(NodeID nid) const;

    bool hasSolvedChildren(NodeID nid) const;

    bool hasOpenChildren(NodeID nid) const;

    /// return whether the node is open or has open children
    bool isOpen(NodeID nid) const;

    /// ********************************************************************

    template<typename Callback>
    void preOrderApply(NodeID start, Callback cb) const;


signals:

    void structureUpdated();

    /// sets immediate children and nodes up the tree as dirty
    void childrenStructureChanged(NodeID nid);

    void node_stats_changed();

};


template<typename Callback>
void NodeTree::preOrderApply(NodeID start, Callback cb) const {
    std::stack<NodeID> stk;

    stk.push(start);

    while(stk.size() > 0) {
        auto nid = stk.top(); stk.pop();

        cb(nid);

        for (auto i = childrenCount(nid) - 1; i >= 0; --i) {
            auto child = getChild_safe(nid, i);
            stk.push(child);
        }
    }
}

}}




#endif
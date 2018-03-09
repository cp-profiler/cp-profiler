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
        emit stats_changed();
        m_branch = m_branch + n;
    }

    void subtract_undetermined(int n) {
        emit stats_changed();
        m_undetermined = m_undetermined - n;
    }

    void add_undetermined(int n) {
        emit stats_changed();
        m_undetermined = m_undetermined + n;
    }

    void add_failed(int n) {
        emit stats_changed();
        m_failed = m_failed + n;
    }

    void add_solved(int n) {
        emit stats_changed();
        m_solved = m_solved + n;
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

    std::unique_ptr<NodeInfo> m_node_info;

    std::vector<Label> m_labels;

    NodeStats m_node_stats;

    /// update data structures to contain this node
    void addEntry(NodeID nid);

public:

    NodeTree();
    ~NodeTree();

    const tree::Structure& tree_structure() const;
    tree::Structure& tree_structure();

    const NodeInfo& node_info() const;
    NodeInfo& node_info();

    NodeStatus status(NodeID nid) const;

    NodeID addNode(NodeID parent_id, int alt, int kids, tree::NodeStatus status, Label = emptyLabel);

    /// Total number of nodes (including undetermined)
    int nodeCount() const;

    const NodeStats& node_stats() const;

    /// return the depth of the tree
    int depth() const;

    int getAlternative(NodeID nid) const;

    int getNumberOfChildren(NodeID nid) const;

    NodeID getParent(NodeID nid) const;

    NodeID getChild(NodeID nid, int alt) const;

    /// return the depth of the node
    int calculateDepth(NodeID nid) const;

    const Label& getLabel(NodeID nid) const;

    void setLabel(NodeID nid, const Label& label);

    template<typename Callback>
    void preOrderApply(NodeID start, Callback cb) const;


signals:

    void structureUpdated();

    void nodesCreated(int count);

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

        for (auto i = getNumberOfChildren(nid) - 1; i >= 0; --i) {
            auto child = getChild(nid, i);
            stk.push(child);
        }
    }
}

}}




#endif
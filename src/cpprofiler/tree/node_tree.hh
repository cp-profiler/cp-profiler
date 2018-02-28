#ifndef CPPROFILER_TREE_NODE_TREE_HH
#define CPPROFILER_TREE_NODE_TREE_HH

#include <QObject>
#include <memory>
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

    void subtract_branch(int n) {
        emit stats_changed();
        m_branch = m_branch - n;
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

    void subtract_failed(int n) {
        emit stats_changed();
        m_failed = m_failed - n;
    }

    void add_failed(int n) {
        emit stats_changed();
        m_failed = m_failed + n;
    }

    void subtract_solved(int n) {
        emit stats_changed();
        m_solved = m_solved - n;
    }

    void add_solved(int n) {
        emit stats_changed();
        m_solved = m_solved + n;
    }

signals:

    void stats_changed();


};

class NodeTree : public QObject {
Q_OBJECT
    std::unique_ptr<Structure> m_structure;


    ///
    std::unique_ptr<NodeInfo> m_node_info;

    NodeStats m_node_stats;

public:

    NodeTree();
    ~NodeTree();

    const tree::Structure& tree_structure() const;
    tree::Structure& tree_structure();

    const NodeInfo& node_info() const;
    NodeInfo& node_info();

    NodeID addNode(NodeID parent_id, int alt, int kids, tree::NodeStatus status);

    /// Total number of nodes (including undetermined)
    int nodeCount() const;

    const NodeStats& node_stats() const;


signals:

    void structureUpdated();

    void nodesCreated(int count);

    void childrenStructureChanged(NodeID nid);

    void node_stats_changed();



};

}}




#endif
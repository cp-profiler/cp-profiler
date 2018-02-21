#ifndef CPPROFILER_TREE_NODE_TREE_HH
#define CPPROFILER_TREE_NODE_TREE_HH

#include <QObject>
#include <memory>
#include "node_id.hh"
#include "node.hh"

namespace cpprofiler { namespace tree {

class Structure;
class NodeInfo;

class NodeTree : public QObject {
Q_OBJECT
    std::unique_ptr<Structure> m_structure;

    std::unique_ptr<NodeInfo> m_node_info;

public:

    NodeTree();
    ~NodeTree();

    const tree::Structure& tree_structure() const;
    tree::Structure& tree_structure();

    const NodeInfo& node_info() const;
    NodeInfo& node_info();

    NodeID addNode(NodeID parent_id, int alt, int kids, tree::NodeStatus status);


signals:

    void structureUpdated();



};

}}




#endif
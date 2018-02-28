#ifndef CPPROFILER_TREE_NODE_INFO
#define CPPROFILER_TREE_NODE_INFO

#include "../global.hh"

#include <bitset>
#include <vector>
#include <QMutex>
#include "node_id.hh"

namespace cpprofiler { namespace tree {

enum class NodeStatus;

enum class NodeFlag {

};

struct NumFlagLoc {
    int pos;
    int len;
};

class NodeInfoEntry {

private:

    std::bitset<32> m_bitset;

public:

    void setFlag(NodeFlag flag, bool value);
    bool getFlag(NodeFlag flag) const;

    void setNumericFlag(NumFlagLoc loc, int value);
    int getNumericFlag(NumFlagLoc loc) const;

};

class NodeInfo {

    mutable utils::Mutex m_mutex;

    std::vector<NodeInfoEntry> m_flags;

public:

    NodeStatus getStatus(NodeID nid) const;
    void setStatus(NodeID nid, NodeStatus status);

    void addEntry(NodeID nid);



};

}}

#endif
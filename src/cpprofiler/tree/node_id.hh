#ifndef CPPROFILER_TREE_NODE_ID_HH
#define CPPROFILER_TREE_NODE_ID_HH

namespace cpprofiler { namespace tree {

struct NodeID {
    int id;

    operator int() const { return id; }
    explicit NodeID(int nid = -1): id(nid) { }
};


}}


#endif
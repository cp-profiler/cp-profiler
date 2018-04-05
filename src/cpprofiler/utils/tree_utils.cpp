#include "tree_utils.hh"
#include <stack>
#include <exception>
#include "../tree/node_tree.hh"

using namespace cpprofiler::tree;

namespace cpprofiler { namespace utils {

int count_descendants(const NodeTree& nt, NodeID nid) {

    if (nid == NodeID::NoNode) {
        throw std::exception();
    }

    std::stack<NodeID> stk;

    stk.push(nid);

    int count = 0;

    while (!stk.empty()) {
        ++count;
        auto n = stk.top(); stk.pop();
        for (auto alt = 0; alt < nt.childrenCount(n); ++alt) {
            stk.push(nt.getChild(n, alt));
        }
    }

    return count;

}


}}
#include "tree_utils.hh"
#include <stack>
#include <exception>
#include "../tree/node_tree.hh"

using namespace cpprofiler::tree;

namespace cpprofiler { namespace utils {

int count_descendants(const NodeTree& nt, NodeID nid) {

    int count = 0;

    auto fun = [&count](NodeID n) {
        count++;
    };

    apply_below(nt, nid, fun);

    return count;

}

void apply_below(const NodeTree& nt, NodeID nid, const NodeAction& action) {

    if (nid == NodeID::NoNode) {
        throw std::exception();
    }

    std::stack<NodeID> stk;

    stk.push(nid);

    while(!stk.empty()) {

        const auto n = stk.top(); stk.pop();
        action(n);

        const auto kids = nt.childrenCount(n);
        for (auto alt = 0; alt < kids; ++alt) {
            stk.push(nt.getChild(n, alt));
        }
    }
}


}}
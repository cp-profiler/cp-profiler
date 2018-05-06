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

int calculate_depth(const NodeTree& nt, NodeID nid) {
    int depth = 0;
    while (nid != NodeID::NoNode) {
        nid = nt.getParent(nid);
        ++depth;
    }
    return depth;
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

void pre_order_apply(const tree::NodeTree& nt, NodeID start, const NodeAction& action) {
    std::stack<NodeID> stk;

    stk.push(start);

    while(stk.size() > 0) {
        auto nid = stk.top(); stk.pop();

        action(nid);

        for (auto i = nt.childrenCount(nid) - 1; i >= 0; --i) {
            auto child = nt.getChild(nid, i);
            stk.push(child);
        }
    }
}

bool is_right_most_child(const tree::NodeTree& nt, NodeID nid) {
    const auto pid = nt.getParent(nid);

    /// root is treated as the left-most child
    if (pid == NodeID::NoNode) return false;

    const auto kids = nt.childrenCount(pid);
    const auto alt = nt.getAlternative(nid);
    return (alt == kids-1);
}

std::vector<NodeID> pre_order(const NodeTree& tree) {
    std::stack<NodeID> stk;
    std::vector<NodeID> result;

    NodeID root = NodeID{0};

    stk.push(root);

    while(stk.size() > 0) {
        auto nid = stk.top(); stk.pop();
        result.push_back(nid);

        for (auto i = tree.childrenCount(nid) - 1; i >= 0 ; --i) {
            auto child = tree.getChild(nid, i);
            stk.push(child);
        }
    }

    return result;
}

std::vector<NodeID> any_order(const NodeTree& tree) {

    auto count = tree.nodeCount();
    std::vector<NodeID> result;
    result.reserve(count);

    for (auto i = 0; i < count; ++i) {
        result.push_back(NodeID(i));
    }

    return result;
}

std::vector<NodeID> post_order(const NodeTree& tree) {
    /// post-order traversal requires two stacks
    std::stack<NodeID> stk_1;
    std::vector<NodeID> result;

    result.reserve(tree.nodeCount());

    NodeID root = NodeID{0};

    stk_1.push(root);

    while(stk_1.size() > 0) {
        auto nid = stk_1.top(); stk_1.pop();

        result.push_back(nid);

        for (auto i = 0; i < tree.childrenCount(nid); ++i) {
            auto child = tree.getChild(nid, i);
            stk_1.push(child);
        }
    }

    std::reverse(result.begin(), result.end());

    return result;
}


}}
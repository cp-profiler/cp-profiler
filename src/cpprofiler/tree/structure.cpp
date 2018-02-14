#include "structure.hh"

#include <iostream>
#include <exception>
#include <stack>
#include <algorithm>

namespace cpprofiler { namespace tree {

    Structure::Structure() {
        std::cerr << "Structure()\n";
        m_nodes.reserve(100);


    }

    NodeID Structure::createRoot(int kids) {

        if (m_nodes.size() > 0) {
            throw invalid_tree();
        }

        m_nodes.push_back(std::unique_ptr<Node>{new Node(kids)});

        return NodeID{0};

    }

    NodeID Structure::addChild(NodeID pid, int alt, int kids) {

        auto nid = NodeID{m_nodes.size()};
        m_nodes.push_back(std::unique_ptr<Node>{new Node(kids)});

        auto p_node = m_nodes[pid].get();
        p_node->setChild(nid, alt);

        return nid;


    }

    NodeID Structure::getChild(NodeID pid, int alt) const {
        return m_nodes[pid]->getChild(alt);
    }

    int Structure::getNumberOfChildren(NodeID pid) const {
        return m_nodes[pid]->getNumberOfChildren();
    }

    int Structure::nodeCount() const {
        return m_nodes.size();
    }

}}


namespace cpprofiler { namespace tree { namespace helper {


std::vector<NodeID> preOrder(const Structure& tree) {
    std::stack<NodeID> stk;
    std::vector<NodeID> result;

    NodeID root = NodeID{0};

    stk.push(root);

    while(stk.size() > 0) {
        auto nid = stk.top(); stk.pop();
        result.push_back(nid);

        for (auto i = tree.getNumberOfChildren(nid) - 1; i >= 0 ; --i) {
            auto child = tree.getChild(nid, i);
            stk.push(child);
        }
    }

    return result;
}

std::vector<NodeID> postOrder(const Structure& tree) {

  /// PO-traversal requires two stacks
  std::stack<NodeID> stk_1;
  std::vector<NodeID> result;

    NodeID root = NodeID{0};

    stk_1.push(root);

    while(stk_1.size() > 0) {
        auto nid = stk_1.top(); stk_1.pop();

        result.push_back(nid);

        for (auto i = 0; i < tree.getNumberOfChildren(nid); ++i) {
            auto child = tree.getChild(nid, i);
            stk_1.push(child);
        }
    }

    std::reverse(result.begin(), result.end());

    return result;

}

}}}
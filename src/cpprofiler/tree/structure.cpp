#include "structure.hh"

#include <iostream>
#include <exception>
#include <stack>
#include <algorithm>
#include <QDebug>

using cpprofiler::utils::Mutex;

namespace cpprofiler { namespace tree {

    Structure::Structure() {
        m_nodes.reserve(100);
    }

    Mutex& Structure::getMutex() const {
        return m_structure_mutex;
    }

    NodeID Structure::createRoot(int kids) {
        if (m_nodes.size() > 0) {
            throw invalid_tree();
        }

        m_nodes.push_back(std::unique_ptr<Node>{new Node(NodeID::NoNode, kids)});

        auto root_nid = getRoot();
        /// create `kids` white nodes
        for (auto i = 0; i < kids; ++i) {
            addChild(root_nid, i, 0);
        }

        return root_nid;
    }

    NodeID Structure::addChild(NodeID pid, int alt, int kids) {
        auto nid = NodeID{m_nodes.size()};
        m_nodes.push_back(std::unique_ptr<Node>{new Node(pid, kids)});

        auto p_node = m_nodes[pid].get();
        p_node->setChild(nid, alt);

        if (p_node->getChild(alt) != nid) throw;

        return nid;
    }

    NodeID Structure::addChild_safe(NodeID pid, int alt, int kids) {
        utils::MutexLocker locker(&m_structure_mutex);
        return addChild(pid, alt, kids);
    }

    void Structure::resetNumberOfChildren_safe(NodeID nid, int kids) {
        utils::MutexLocker locker(&m_structure_mutex);
       resetNumberOfChildren(nid, kids);
    }

    void Structure::resetNumberOfChildren(NodeID nid, int kids) {
        m_nodes[nid]->resetNumberOfChildren(kids);

        for (auto i = 0; i < kids; ++i) {
            addChild(nid, i, 0);
        }
    }

    NodeID Structure::getChild(NodeID pid, int alt) const {
        return m_nodes[pid]->getChild(alt);
    }

    NodeID Structure::getChild_safe(NodeID pid, int alt) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return getChild(pid, alt);;
    }

    NodeID Structure::getParent(NodeID nid) const {
        return m_nodes[nid]->getParent();
    }

    NodeID Structure::getParent_safe(NodeID nid) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return getParent(nid);
    }

    int Structure::childrenCount(NodeID pid) const {
        return m_nodes[pid]->childrenCount();
    }

    int Structure::childrenCount_safe(NodeID pid) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return childrenCount(pid);
    }


    int Structure::getNumberOfSiblings_safe(NodeID nid) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return getNumberOfSiblings(nid);
    }

    int Structure::getNumberOfSiblings(NodeID nid) const {
        auto pid = getParent(nid);
        return childrenCount(pid);
    }

    NodeID Structure::getRoot() const {
        return NodeID{0};
    }

    int Structure::getAlternative_safe(NodeID nid) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return getAlternative(nid);
    }

    int Structure::getAlternative(NodeID nid) const {
        auto parent_nid = getParent(nid);

        if (parent_nid == NodeID::NoNode) return -1;

        for (auto i = 0; i < childrenCount(parent_nid); ++i) {
            if (getChild(parent_nid, i) == nid) {
                return i;
            }
        }
        throw;
        return -1;
    }

    int Structure::nodeCount_safe() const {
        utils::MutexLocker locker(&m_structure_mutex);
        return nodeCount();
    }

    int Structure::nodeCount() const {
        return m_nodes.size();
    }

    int Structure::calculateDepth_safe(NodeID nid) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return calculateDepth(nid);
    }

    int Structure::calculateDepth(NodeID nid) const {
        int depth = 0;
        while (nid != NodeID::NoNode) {
            nid = getParent(nid);
            ++depth;
        }
        return depth;
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

        for (auto i = tree.childrenCount_safe(nid) - 1; i >= 0 ; --i) {
            auto child = tree.getChild_safe(nid, i);
            stk.push(child);
        }
    }

    return result;
}

std::vector<NodeID> postOrder(const Structure& tree) {
    utils::MutexLocker lock(&tree.getMutex());
    return postOrder_unsafe(tree);

}

std::vector<NodeID> anyOrder(const Structure& tree) {
    utils::MutexLocker lock(&tree.getMutex());

    auto count = tree.nodeCount();
    std::vector<NodeID> result;
    result.reserve(count);

    for (auto i = 0; i < count; ++i) {
        result.push_back(NodeID(i));
    }

    return result;
}

std::vector<NodeID> postOrder_unsafe(const Structure& tree) {
    /// PO-traversal requires two stacks
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

}}}
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
        utils::MutexLocker locker(&m_structure_mutex);
        if (m_nodes.size() > 0) {
            throw invalid_tree();
        }

        m_nodes.push_back(std::unique_ptr<Node>{new Node(NodeID::NoNode, kids)});

        auto root_nid = getRoot_unsafe();
        /// create `kids` white nodes
        for (auto i = 0; i < kids; ++i) {
            addChild_unsafe(root_nid, i, 0);
        }

        return root_nid;
    }

    NodeID Structure::addChild_unsafe(NodeID pid, int alt, int kids) {
        auto nid = NodeID{m_nodes.size()};
        m_nodes.push_back(std::unique_ptr<Node>{new Node(pid, kids)});

        auto p_node = m_nodes[pid].get();
        p_node->setChild(nid, alt);

        if (p_node->getChild(alt) != nid) throw;

        return nid;
    }

    NodeID Structure::addChild(NodeID pid, int alt, int kids) {
        utils::MutexLocker locker(&m_structure_mutex);
        return addChild_unsafe(pid, alt, kids);
    }

    void Structure::resetNumberOfChildren(NodeID nid, int kids) {
        utils::MutexLocker locker(&m_structure_mutex);
        m_nodes[nid]->resetNumberOfChildren(kids);

        for (auto i = 0; i < kids; ++i) {
            addChild_unsafe(nid, i, 0);
        }
    }

    NodeID Structure::getChild_unsafe(NodeID pid, int alt) const {
        return m_nodes[pid]->getChild(alt);
    }

    NodeID Structure::getChild(NodeID pid, int alt) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return getChild_unsafe(pid, alt);;
    }

    NodeID Structure::getParent_unsafe(NodeID nid) const {
        return m_nodes[nid]->getParent();
    }

    NodeID Structure::getParent(NodeID nid) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return getParent_unsafe(nid);
    }

    int Structure::getNumberOfChildren_unsafe(NodeID pid) const {
        return m_nodes[pid]->getNumberOfChildren();
    }

    int Structure::getNumberOfChildren(NodeID pid) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return getNumberOfChildren_unsafe(pid);
    }


    int Structure::getNumberOfSiblings(NodeID nid) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return getNumberOfSiblings_unsafe(nid);
    }

    int Structure::getNumberOfSiblings_unsafe(NodeID nid) const {
        auto pid = getParent_unsafe(nid);
        return getNumberOfChildren_unsafe(pid);
    }

    NodeID Structure::getRoot_unsafe() const {
        return NodeID{0};
    }

    NodeID Structure::getRoot() const {
        utils::MutexLocker locker(&m_structure_mutex);
        return getRoot_unsafe();
    }

    int Structure::getAlternative(NodeID nid) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return getAlternative_unsafe(nid);
    }

    int Structure::getAlternative_unsafe(NodeID nid) const {
        auto parent_nid = getParent_unsafe(nid);

        if (parent_nid == NodeID::NoNode) return -1;

        for (auto i = 0; i < getNumberOfChildren_unsafe(parent_nid); ++i) {
            if (getChild_unsafe(parent_nid, i) == nid) {
                return i;
            }
        }
        throw;
        return -1;
    }

    int Structure::nodeCount() const {
        utils::MutexLocker locker(&m_structure_mutex);
        return nodeCount_unsafe();
    }

    int Structure::nodeCount_unsafe() const {
        return m_nodes.size();
    }

    int Structure::calculateDepth(NodeID nid) const {
        utils::MutexLocker locker(&m_structure_mutex);
        return calculateDepth_unsafe(nid);
    }

    int Structure::calculateDepth_unsafe(NodeID nid) const {
        int depth = 0;
        while (nid != NodeID::NoNode) {
            nid = getParent_unsafe(nid);
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

        for (auto i = tree.getNumberOfChildren(nid) - 1; i >= 0 ; --i) {
            auto child = tree.getChild(nid, i);
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

    auto count = tree.nodeCount_unsafe();
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

    result.reserve(tree.nodeCount_unsafe());

    NodeID root = NodeID{0};

    stk_1.push(root);

    while(stk_1.size() > 0) {
        auto nid = stk_1.top(); stk_1.pop();

        result.push_back(nid);

        for (auto i = 0; i < tree.getNumberOfChildren_unsafe(nid); ++i) {
            auto child = tree.getChild_unsafe(nid, i);
            stk_1.push(child);
        }
    }

    std::reverse(result.begin(), result.end());

    return result;
}

}}}
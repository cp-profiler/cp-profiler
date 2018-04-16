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
            createChild(root_nid, i, 0);
        }

        return root_nid;
    }

    NodeID Structure::createNode(NodeID pid, int kids) {
        auto nid = NodeID{m_nodes.size()};
        m_nodes.push_back(std::unique_ptr<Node>{new Node(pid, kids)});
        return nid;
    }

    NodeID Structure::createChild(NodeID pid, int alt, int kids) {
        const auto nid = createNode(pid, kids);
        m_nodes[pid]->setChild(nid, alt);
        return nid;
    }

    NodeID Structure::addExtraChild(NodeID pid) {

        const auto alt = childrenCount(pid);

        /// make room for another child
        m_nodes[pid]->addChild();

        auto kid = createChild(pid, alt, 0);
        return kid;
    }

    void Structure::addChildren(NodeID nid, int kids) {
        if (m_nodes[nid]->childrenCount() > 0) throw;

        m_nodes[nid]->setNumberOfChildren(kids);

        for (auto i = 0; i < kids; ++i) {
            createChild(nid, i, 0);
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

        for (auto i = tree.childrenCount(nid) - 1; i >= 0 ; --i) {
            auto child = tree.getChild_safe(nid, i);
            stk.push(child);
        }
    }

    return result;
}

std::vector<NodeID> anyOrder(const Structure& tree) {

    auto count = tree.nodeCount();
    std::vector<NodeID> result;
    result.reserve(count);

    for (auto i = 0; i < count; ++i) {
        result.push_back(NodeID(i));
    }

    return result;
}

std::vector<NodeID> postOrder(const Structure& tree) {
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
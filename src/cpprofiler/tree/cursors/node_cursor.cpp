#include "node_cursor.hh"

#include "../structure.hh"
#include "../node_tree.hh"
#include <QDebug>

namespace cpprofiler { namespace tree {

    NodeCursor::NodeCursor(NodeID start, const NodeTree& tree)
    : m_tree(tree.tree_structure()), m_node_info(tree.node_info()), m_start_node(start), m_cur_node(start), m_cur_alt(0) {

    }

    bool NodeCursor::mayMoveDownwards() const {
        auto n = m_tree.childrenCount_safe(m_cur_node);
        return (n > 0);
    }

    void NodeCursor::moveDownwards() {
        m_cur_alt = 0;
        m_cur_node = m_tree.getChild_safe(m_cur_node, 0);
    }

    bool NodeCursor::mayMoveSidewards() const {
        return (m_cur_node != m_start_node)
            && (m_cur_node != m_tree.getRoot_safe())
            && (m_cur_alt < m_tree.getNumberOfSiblings_safe(m_cur_node) - 1);
    }

    void NodeCursor::moveSidewards() {
        auto parent_nid = m_tree.getParent_safe(m_cur_node);
        m_cur_node = m_tree.getChild_safe(parent_nid, ++m_cur_alt);
    }

    bool NodeCursor::mayMoveUpwards() const {
        return (m_cur_node != m_start_node) && (m_cur_node != m_tree.getRoot_safe());
    }

    void NodeCursor::moveUpwards() {

        m_cur_node = m_tree.getParent_safe(m_cur_node);
        m_cur_alt = m_tree.getAlternative_safe(m_cur_node);
    }

    void NodeCursor::finalize() {
        /// do nothing
    }


}}

namespace cpprofiler { namespace tree {

    UnsafeNodeCursor::UnsafeNodeCursor(NodeID start, const NodeTree& tree)
    : m_tree(tree.tree_structure()), m_node_info(tree.node_info()), m_start_node(start), m_cur_node(start), m_cur_alt(0) {

    }

    bool UnsafeNodeCursor::mayMoveDownwards() const {
        auto n = m_tree.childrenCount(m_cur_node);
        return (n > 0);
    }

    void UnsafeNodeCursor::moveDownwards() {
        m_cur_alt = 0;
        m_cur_node = m_tree.getChild(m_cur_node, 0);
    }

    bool UnsafeNodeCursor::mayMoveSidewards() const {
        return (m_cur_node != m_start_node)
            && (m_cur_node != m_tree.getRoot())
            && (m_cur_alt < m_tree.getNumberOfSiblings(m_cur_node) - 1);
    }

    void UnsafeNodeCursor::moveSidewards() {
        auto parent_nid = m_tree.getParent(m_cur_node);
        m_cur_node = m_tree.getChild(parent_nid, ++m_cur_alt);
    }

    bool UnsafeNodeCursor::mayMoveUpwards() const {
        return (m_cur_node != m_start_node) && (m_cur_node != m_tree.getRoot());
    }

    void UnsafeNodeCursor::moveUpwards() {

        m_cur_node = m_tree.getParent(m_cur_node);
        m_cur_alt = m_tree.getAlternative(m_cur_node);
    }

    void UnsafeNodeCursor::finalize() {
        /// do nothing
    }


}}
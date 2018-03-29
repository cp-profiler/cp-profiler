#ifndef CPPROFILER_TREE_LAYOUT_COMPUTER_HH
#define CPPROFILER_TREE_LAYOUT_COMPUTER_HH

class QMutex;

#include "node_id.hh"

namespace cpprofiler { namespace tree {

    class Layout;
    class NodeTree;
    class NodeFlags;

    class LayoutComputer {

        const NodeTree& m_tree;
        const NodeFlags& m_flags;
        Layout& m_layout;

        bool m_needs_update = true;
public:

        LayoutComputer(const NodeTree& tree, Layout& layout, const NodeFlags& nf);

        bool compute();

        void markAsOutdated();

        void dirtyUp(NodeID nid);

        void setDirty(NodeID nid);
    };


}}


#endif
#ifndef CPPROFILER_TREE_LAYOUT_COMPUTER_HH
#define CPPROFILER_TREE_LAYOUT_COMPUTER_HH

class QMutex;

namespace cpprofiler { namespace tree {

    class Layout;
    class NodeTree;
    class NodeFlags;

    class LayoutComputer {

        const NodeTree& m_node_tree;
        const NodeFlags& m_flags;
        Layout& m_layout;

        bool m_needs_update = true;
public:

        LayoutComputer(const NodeTree& tree, Layout& layout, const NodeFlags& nf);

        bool compute();

        void markAsOutdated();
    };


}}


#endif
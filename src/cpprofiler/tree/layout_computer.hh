#ifndef CPPROFILER_TREE_LAYOUT_COMPUTER_HH
#define CPPROFILER_TREE_LAYOUT_COMPUTER_HH

class QMutex;

namespace cpprofiler { namespace tree {

    class Layout;
    class Structure;
    class NodeFlags;

    class LayoutComputer {

        const Structure& m_tree;
        const NodeFlags& m_flags;
        Layout& m_layout;

        bool m_needs_update = true;
public:

        LayoutComputer(const Structure& tree, Layout& layout, const NodeFlags& nf);

        bool compute();

        void markAsOutdated();
    };


}}


#endif
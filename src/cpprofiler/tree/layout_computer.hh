#ifndef CPPROFILER_TREE_LAYOUT_COMPUTER_HH
#define CPPROFILER_TREE_LAYOUT_COMPUTER_HH

class QMutex;

namespace cpprofiler { namespace tree {

    class Layout;
    class Structure;

    class LayoutComputer {

        const Structure& m_tree;
        Layout& m_layout;

        bool m_needs_update = true;
public:

        LayoutComputer(const Structure& tree, Layout& layout);

        bool compute();

        void markAsOutdated();
    };


}}


#endif
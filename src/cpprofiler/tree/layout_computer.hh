#ifndef CPPROFILER_TREE_LAYOUT_COMPUTER_HH
#define CPPROFILER_TREE_LAYOUT_COMPUTER_HH

class QMutex;

namespace cpprofiler { namespace tree {

    class Layout;
    class Structure;

    class LayoutComputer {

        const Structure& m_tree;
        Layout& m_layout;
        QMutex& m_layout_mutex;


public:

        LayoutComputer(const Structure& tree, Layout& layout, QMutex& layout_mutex);

        void compute();
    };


}}


#endif
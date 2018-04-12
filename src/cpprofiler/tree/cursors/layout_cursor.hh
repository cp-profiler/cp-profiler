#ifndef CPPROFILER_TREE_CURSORS_LAYOUT_CURSOR
#define CPPROFILER_TREE_CURSORS_LAYOUT_CURSOR

#include <vector>
#include <QPainter>
#include "node_cursor.hh"

namespace cpprofiler { namespace tree {

    class Layout;
    class VisualFlags;

class LayoutCursor : public NodeCursor {

    Layout& m_layout;
    const NodeTree& m_nt;
    const Structure& m_tree;
    const VisualFlags& m_vis_flags;
    /// painter used for dispaying text (labels)
    const QPainter* m_painter = nullptr;

public:
    // Constructor
    LayoutCursor(NodeID start, const NodeTree& tree, const VisualFlags& nf, Layout& lo);

    void setLabelPainter(QPainter* painter);

    void computeForNode(NodeID nid);

    bool mayMoveDownwards();

    void processCurrentNode();

    void finalize();

};

}}


#endif
#ifndef CPPROFILER_TREE_CURSORS_DRAWING_CURSOR_HH
#define CPPROFILER_TREE_CURSORS_DRAWING_CURSOR_HH

#include "node_cursor.hh"
#include "../layout.hh"
#include <QPoint>
#include <QRect>


class QPainter;

namespace cpprofiler {
    class UserData;

    namespace tree {
        class VisualFlags;
    }
}

namespace cpprofiler { namespace tree {

class Layout;

/// This uses unsafe methods for tree structure!
class DrawingCursor : public UnsafeNodeCursor {

    const Layout& m_layout;

    const NodeTree& m_node_tree;

    const UserData& m_user_data;

    const VisualFlags& m_vis_flags;

    QPainter& m_painter;
    QRect clippingRect;

    int cur_x, cur_y;

    bool isClipped();

public:

    DrawingCursor(NodeID start,
                 const NodeTree& tree,
                 const Layout& layout,
                 const UserData& user_data,
                 const VisualFlags& flags,
                 QPainter& painter,
                 QPoint start_pos,
                 const QRect& clippingRect0);

    void processCurrentNode();

    bool mayMoveUpwards();
    bool mayMoveSidewards();
    bool mayMoveDownwards();

    void moveUpwards();
    void moveDownwards();
    void moveSidewards();

    void finalize();

};


}}


#endif
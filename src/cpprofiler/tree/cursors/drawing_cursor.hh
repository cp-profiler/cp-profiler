#ifndef CPPROFILER_TREE_CURSORS_DRAWING_CURSOR_HH
#define CPPROFILER_TREE_CURSORS_DRAWING_CURSOR_HH

#include "node_cursor.hh"
#include <QPoint>
#include <QRect>


class QPainter;

namespace cpprofiler {
    class UserData;

    namespace tree {
        class NodeFlags;
    }
}

namespace cpprofiler { namespace tree {

namespace traditional {
    constexpr int NODE_WIDTH = 20;
    constexpr int FAILED_WIDTH = 14;
    constexpr int HALF_FAILED_WIDTH = FAILED_WIDTH / 2;
    constexpr int HALF_NODE_WIDTH = NODE_WIDTH / 2;
    constexpr int SHADOW_OFFSET = 3.0;
}

class Layout;

class DrawingCursor : public NodeCursor {

    const Layout& m_layout;

    const UserData& m_user_data;

    const NodeFlags& m_flags;


    QPainter& m_painter;
    QRect clippingRect;

    int cur_x, cur_y;

    bool isClipped();

public:

    DrawingCursor(NodeID start,
                 const NodeTree& tree,
                 const Layout& layout,
                 const UserData& user_data,
                 const NodeFlags& flags,
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
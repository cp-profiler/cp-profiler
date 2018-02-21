#ifndef CPPROFILER_TREE_CURSORS_DRAWING_CURSOR_HH
#define CPPROFILER_TREE_CURSORS_DRAWING_CURSOR_HH

#include "node_cursor.hh"
#include <QPoint>


class QPainter;

namespace cpprofiler {
    class UserData;

    namespace tree {
        class NodeFlags;
    }
}

namespace cpprofiler { namespace tree {

class Layout;

class DrawingCursor : public NodeCursor {

    const Layout& m_layout;

    const UserData& m_user_data;

    const NodeFlags& m_flags;

    QPainter& m_painter;

    int cur_x, cur_y;

public:

    DrawingCursor(NodeID start, const NodeTree& tree,
        const Layout& layout, const UserData& user_data, const NodeFlags& flags, QPainter& painter, QPoint start_pos);

    void processCurrentNode();

    bool mayMoveDownwards();

    void moveUpwards();
    void moveDownwards();
    void moveSidewards();

};


}}


#endif
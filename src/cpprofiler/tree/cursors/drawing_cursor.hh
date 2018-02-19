#ifndef CPPROFILER_TREE_CURSORS_DRAWING_CURSOR_HH
#define CPPROFILER_TREE_CURSORS_DRAWING_CURSOR_HH

#include "node_cursor.hh"


class QPainter;

namespace cpprofiler {
    class UserData;
}

namespace cpprofiler { namespace tree {

class Layout;

class DrawingCursor : public NodeCursor {

    double cur_x = 200, cur_y = 50;

    const Layout& m_layout;

    const UserData& m_user_data;

    QPainter& m_painter;

public:

    DrawingCursor(NodeID start, const NodeTree& tree,
        const Layout& layout, const UserData& user_data, QPainter& painter);

    void processCurrentNode();

    void moveUpwards();
    void moveDownwards();
    void moveSidewards();

};


}}


#endif
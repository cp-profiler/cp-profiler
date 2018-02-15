#ifndef CPPROFILER_TREE_CURSORS_DRAWING_CURSOR_HH
#define CPPROFILER_TREE_CURSORS_DRAWING_CURSOR_HH

#include "node_cursor.hh"


class QPainter;

namespace cpprofiler { namespace tree {

class Layout;

class DrawingCursor : public NodeCursor {

    double cur_x = 100, cur_y = 50;

    const Layout& m_layout;

    QPainter& m_painter;

public:

    DrawingCursor(NodeID start, const Structure& tree, const Layout& layout, QPainter& painter);

    void processCurrentNode();

    void moveUpwards();
    void moveDownwards();
    void moveSidewards();

};


}}


#endif
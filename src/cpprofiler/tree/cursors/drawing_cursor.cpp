#include "drawing_cursor.hh"
#include "../layout.hh"

#include <QDebug>
#include <QPainter>

namespace cpprofiler { namespace tree {

    DrawingCursor::DrawingCursor(NodeID start, const Structure& tree, const Layout& layout, QPainter& painter)
    : NodeCursor(start, tree), m_layout(layout), m_painter(painter) {}

    void DrawingCursor::processCurrentNode() {

        m_painter.drawEllipse(cur_x, cur_y, 20, 20);
    }

    void DrawingCursor::moveUpwards() {
        cur_x -= m_layout.getOffset(m_cur_node);
        cur_y -= Layout::dist_y;
        NodeCursor::moveUpwards();
    }

    void DrawingCursor::moveDownwards() {
        NodeCursor::moveDownwards();
        cur_x += m_layout.getOffset(m_cur_node);
        cur_y += Layout::dist_y;
    }

    void DrawingCursor::moveSidewards() {
        cur_x -= m_layout.getOffset(m_cur_node);
        NodeCursor::moveSidewards();
        cur_x += m_layout.getOffset(m_cur_node);
    }

}}
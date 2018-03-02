#include "drawing_cursor.hh"
#include "../layout.hh"
#include "../node_info.hh"
#include "../node.hh"
#include "../shape.hh"
#include "../../user_data.hh"
#include "../traditional_view.hh"

#include <QDebug>
#include <QPainter>

namespace cpprofiler { namespace tree {

    namespace colors {
        /// The color for selected nodes
        static QColor gold(252, 209, 22);
        /// Red color for failed nodes
        static QColor red(218, 37, 29);
        /// Green color for solved nodes
        static QColor green(11, 118, 70);
        /// Blue color for choice nodes
        static QColor blue(0, 92, 161);
        /// Grey color for skipped nodes
        static QColor grey(150, 150, 150);
        /// Orange color for best solutions
        static QColor orange(235, 137, 27);
        /// White color
        static QColor white(255,255,255);

        /// Red color for expanded failed nodes
        static QColor lightRed(218, 37, 29, 120);
        /// Green color for expanded solved nodes
        static QColor lightGreen(11, 118, 70, 120);
        /// Blue color for expanded choice nodes
        static QColor lightBlue(0, 92, 161, 120);
    }

    DrawingCursor::DrawingCursor(NodeID start, const NodeTree& tree,
        const Layout& layout, const UserData& user_data, const NodeFlags& flags, QPainter& painter, QPoint start_pos, const QRect& clip)
    : UnsafeNodeCursor(start, tree), m_layout(layout), m_user_data(user_data), m_flags(flags), m_painter(painter), clippingRect(clip) {
        cur_x = start_pos.x(); cur_y = start_pos.y();
    }

    static void drawDiamond(QPainter& painter, int myx, int myy, bool shadow) {
        using namespace traditional;
        QPointF points[4] = { QPointF(myx, myy),
            QPointF(myx + HALF_NODE_WIDTH, myy + HALF_NODE_WIDTH),
            QPointF(myx, myy + NODE_WIDTH),
            QPointF(myx - HALF_NODE_WIDTH, myy + HALF_NODE_WIDTH)
        };

        if (shadow) {
            for (auto&& p : points) { p += QPointF(SHADOW_OFFSET, SHADOW_OFFSET);}
        }

        painter.drawConvexPolygon(points, 4);
    }

    static void drawBranchNode(QPainter& painter, int x, int y, bool selected, bool phantom) {
        using namespace traditional;
        if (selected) {
            painter.setBrush(colors::gold);
        } else {
            painter.setBrush(colors::blue);
        }

        if (phantom) {
            auto brush = painter.brush();
            brush.setStyle(Qt::Dense4Pattern);
            painter.setBrush(brush);
        }
        painter.drawEllipse(x - HALF_NODE_WIDTH, y, NODE_WIDTH, NODE_WIDTH);
    }

    static void drawFailedNode(QPainter& painter, int x, int y, bool selected, bool phantom) {
        using namespace traditional;
        if (selected) {
            painter.setBrush(colors::gold);
        } else {
            painter.setBrush(colors::red);
        }

        if (phantom) {
            auto brush = painter.brush();
            brush.setStyle(Qt::Dense4Pattern);
            painter.setBrush(brush);
        }

        painter.drawRect(x - HALF_FAILED_WIDTH, y, FAILED_WIDTH, FAILED_WIDTH);
        painter.drawRect(x, y, 2, 2);
    }

    static void drawUndeterminedNode(QPainter& painter, int x, int y, bool selected, bool phantom) {
        using namespace traditional;
        if (selected) {
            painter.setBrush(colors::gold);
        } else {
            painter.setBrush(colors::white);
        }

        if (phantom) {
            auto brush = painter.brush();
            brush.setStyle(Qt::Dense4Pattern);
            painter.setBrush(brush);
        }
        painter.drawEllipse(x - HALF_NODE_WIDTH, y, NODE_WIDTH, NODE_WIDTH);
    }

    static void drawSolutionNode(QPainter& painter, int x, int y, bool selected, bool phantom) {
        using namespace traditional;
        if (selected) {
            painter.setBrush(colors::gold);
        } else {
            painter.setBrush(colors::green);
        }

        if (phantom) {
            auto brush = painter.brush();
            brush.setStyle(Qt::Dense4Pattern);
            painter.setBrush(brush);
        }
        drawDiamond(painter, x, y, false);
    }

    static void drawSkippedNode(QPainter& painter, int x, int y, bool selected, bool phantom) {
        using namespace traditional;
        if (selected) {
            painter.setBrush(colors::gold);
        } else {
            painter.setBrush(colors::grey);
        }

        if (phantom) {
            auto brush = painter.brush();
            brush.setStyle(Qt::Dense4Pattern);
            painter.setBrush(brush);
        }
        painter.drawRect(x - HALF_FAILED_WIDTH, y, FAILED_WIDTH, FAILED_WIDTH);
    }

    static void drawShape(QPainter& painter, int x, int y, NodeID nid, const Layout& layout) {
        using namespace traditional;
        painter.setPen(Qt::NoPen);

        auto& shape = layout.getShape_unsafe(nid);

        int depth = shape.depth();
        QPointF *points = new QPointF[depth * 2];

        int l_x = x + shape[0].l;
        int r_x = x + shape[0].r;
        y = y + NODE_WIDTH / 2;

        points[0] = QPointF(l_x, y);
        points[depth * 2 - 1] = QPointF(r_x, y);

        for (int i = 1; i <  depth; i++){
            y += static_cast<double>(Layout::dist_y);
            l_x = x + shape[i].l;
            r_x = x + shape[i].r;
            points[i] = QPointF(l_x, y);
            points[depth * 2 - i - 1] = QPointF(r_x, y);
        }

        painter.drawConvexPolygon(points, shape.depth() * 2);

        delete[] points;
    }

    void DrawingCursor::processCurrentNode() {

        using namespace traditional;

        /// used for debugging
        // bool phantom_node = isClipped();
        bool phantom_node = false;

        m_painter.setPen(QColor{Qt::black});

        // if (phantom_node) {
        //     m_painter.setPen(QColor{colors::grey});
        // }

        if (m_cur_node != m_start_node) {

            auto parent_x = cur_x - m_layout.getOffset_unsafe(m_cur_node);
            auto parent_y = cur_y - static_cast<double>(Layout::dist_y);

            m_painter.drawLine(parent_x, parent_y + NODE_WIDTH, cur_x, cur_y);
        }

        auto status = m_node_info.getStatus(m_cur_node);

        auto selected = (m_user_data.getSelectedNode() == m_cur_node) ? true : false;


        if (m_flags.get_label_shown(m_cur_node)) {
            // m_painter.setPen(QPen{Qt::black, 2});
            m_painter.drawText(QPoint{cur_x, cur_y}, "hello");
        }

        switch (status) {
            case NodeStatus::SOLVED: {
                drawSolutionNode(m_painter, cur_x, cur_y, selected, phantom_node);
            } break;
            case NodeStatus::FAILED: {
                drawFailedNode(m_painter, cur_x, cur_y, selected, phantom_node);
            } break;
            case NodeStatus::BRANCH: {
                drawBranchNode(m_painter, cur_x, cur_y, selected, phantom_node);
            } break;
            case NodeStatus::SKIPPED: {
                drawSkippedNode(m_painter, cur_x, cur_y, selected, phantom_node);
            } break;
            default: {
                drawUndeterminedNode(m_painter, cur_x, cur_y, selected, phantom_node);
            } break;
        }

        /// draw bounding box
        if (m_user_data.getSelectedNode() == m_cur_node) {
            m_painter.setBrush(QColor{0, 0, 0, 20});
            auto bb = m_layout.getBoundingBox_unsafe(m_cur_node);

            auto height = m_layout.getDepth_unsafe(m_cur_node) * Layout::dist_y;
            m_painter.drawRect(cur_x + bb.left, cur_y, bb.right - bb.left, height);


            m_painter.setBrush(QColor{0, 0, 0, 80});
            drawShape(m_painter, cur_x, cur_y, m_cur_node, m_layout);
        }
    }

    void DrawingCursor::moveUpwards() {
        cur_x -= m_layout.getOffset_unsafe(m_cur_node);
        cur_y -= Layout::dist_y;
        UnsafeNodeCursor::moveUpwards();
    }

    void DrawingCursor::moveDownwards() {
        UnsafeNodeCursor::moveDownwards();
        cur_x += m_layout.getOffset_unsafe(m_cur_node);
        cur_y += Layout::dist_y;
    }

    void DrawingCursor::moveSidewards() {
        cur_x -= m_layout.getOffset_unsafe(m_cur_node);
        UnsafeNodeCursor::moveSidewards();
        cur_x += m_layout.getOffset_unsafe(m_cur_node);
    }

    bool DrawingCursor::mayMoveSidewards() {
        return UnsafeNodeCursor::mayMoveSidewards();


    }

    bool DrawingCursor::mayMoveDownwards() {
        /// TODO: this should be about children?
        return UnsafeNodeCursor::mayMoveDownwards() && m_layout.getLayoutDone_unsafe(m_cur_node) && !isClipped();
    }
    
    bool DrawingCursor::mayMoveUpwards() {

        return UnsafeNodeCursor::mayMoveUpwards();
    }

    bool DrawingCursor::isClipped() {
        auto bb = m_layout.getBoundingBox_unsafe(m_cur_node);

        if (
            (cur_x + bb.left > clippingRect.x() + clippingRect.width()) ||
            (cur_x + bb.right < clippingRect.x()) ||
            (cur_y > clippingRect.y() + clippingRect.height()) ||
            (cur_y + (m_layout.getDepth_unsafe(m_cur_node) + 1) * Layout::dist_y < clippingRect.y())
        ) {
            // qDebug() << "node clipped";
            return true;
        }

        return false;

    }

    void DrawingCursor::finalize() {
    }

}}
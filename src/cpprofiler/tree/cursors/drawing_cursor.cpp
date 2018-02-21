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

    namespace traditional {
        constexpr double NODE_WIDTH = 20;
        constexpr double FAILED_WIDTH = 14;
        constexpr double HALF_FAILED_WIDTH = FAILED_WIDTH / 2;
        constexpr double HALF_NODE_WIDTH = NODE_WIDTH / 2;
        constexpr double SHADOW_OFFSET = 3.0;
    }

    DrawingCursor::DrawingCursor(NodeID start, const NodeTree& tree,
        const Layout& layout, const UserData& user_data, const NodeFlags& flags, QPainter& painter, QPoint start_pos)
    : NodeCursor(start, tree), m_layout(layout), m_user_data(user_data), m_flags(flags), m_painter(painter) {
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

    static void drawBranchNode(QPainter& painter, int x, int y, bool selected) {
        using namespace traditional;
        if (selected) {
            painter.setBrush(colors::gold);
        } else {
            painter.setBrush(colors::blue);
        }
        painter.drawEllipse(x - HALF_NODE_WIDTH, y, NODE_WIDTH, NODE_WIDTH);
    }

    static void drawFailedNode(QPainter& painter, int x, int y, bool selected) {
        using namespace traditional;
        if (selected) {
            painter.setBrush(colors::gold);
        } else {
            painter.setBrush(colors::red);
        }
        painter.drawRect(x - HALF_FAILED_WIDTH, y, FAILED_WIDTH, FAILED_WIDTH);
    }

    static void drawUndeterminedNode(QPainter& painter, int x, int y, bool selected) {
        using namespace traditional;
        if (selected) {
            painter.setBrush(colors::gold);
        } else {
            painter.setBrush(colors::white);
        }
        painter.drawEllipse(x - HALF_NODE_WIDTH, y, NODE_WIDTH, NODE_WIDTH);
    }

    static void drawSolutionNode(QPainter& painter, int x, int y, bool selected) {
        using namespace traditional;
        if (selected) {
            painter.setBrush(colors::gold);
        } else {
            painter.setBrush(colors::green);
        }
        drawDiamond(painter, x, y, selected);
    }

    static void drawSkippedNode(QPainter& painter, int x, int y, bool selected) {
        using namespace traditional;
        if (selected) {
            painter.setBrush(colors::gold);
        } else {
            painter.setBrush(colors::grey);
        }
        painter.drawRect(x - HALF_FAILED_WIDTH, y, FAILED_WIDTH, FAILED_WIDTH);
    }

    void DrawingCursor::processCurrentNode() {

        using namespace traditional;

        m_painter.setPen(QColor{Qt::black});

        if (m_cur_node != m_start_node) {

            auto parent_x = cur_x - m_layout.getOffset(m_cur_node);
            auto parent_y = cur_y - static_cast<double>(Layout::dist_y);

            m_painter.drawLine(parent_x, parent_y + NODE_WIDTH, cur_x, cur_y);
        }

        m_painter.setBrush(Qt::white);

        auto status = m_node_info.getStatus(m_cur_node);

        auto selected = (m_user_data.getSelectedNode() == m_cur_node) ? true : false;


        if (m_flags.get_label_shown(m_cur_node)) {
            m_painter.setPen(QPen{Qt::black, 2});
        }

        switch (status) {
            case NodeStatus::SOLVED: {
                drawSolutionNode(m_painter, cur_x, cur_y, selected);
            } break;
            case NodeStatus::FAILED: {
                drawFailedNode(m_painter, cur_x, cur_y, selected);
            } break;
            case NodeStatus::BRANCH: {
                drawBranchNode(m_painter, cur_x, cur_y, selected);
            } break;
            case NodeStatus::SKIPPED: {
                drawSkippedNode(m_painter, cur_x, cur_y, selected);
            } break;
            default: {
                drawUndeterminedNode(m_painter, cur_x, cur_y, selected);
            } break;
        }

        /// draw bounding box
        if (m_user_data.getSelectedNode() == m_cur_node) {
            m_painter.setBrush(QColor{0, 0, 0, 20});
            auto bb = m_layout.getBoundingBox(m_cur_node);

            auto height = m_layout.getDepth(m_cur_node) * Layout::dist_y;
            m_painter.drawRect(cur_x + bb.left, cur_y, bb.right - bb.left, height);
        }
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

    bool DrawingCursor::mayMoveDownwards() {
        /// TODO: this should be about children?
        return NodeCursor::mayMoveDownwards() &&
                m_layout.getLayoutDone(m_cur_node);
    }

}}
#include "drawing_cursor.hh"
#include "../layout.hh"
#include "../node_info.hh"
#include "../node.hh"
#include "../../user_data.hh"

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
        const Layout& layout, const UserData& user_data, QPainter& painter)
    : NodeCursor(start, tree), m_layout(layout), m_user_data(user_data), m_painter(painter) {}

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

    void DrawingCursor::processCurrentNode() {

        using namespace traditional;

        if (m_cur_node != m_start_node) {

            auto parent_x = cur_x - m_layout.getOffset(m_cur_node);
            auto parent_y = cur_y - static_cast<double>(Layout::dist_y);

            m_painter.drawLine(parent_x, parent_y + NODE_WIDTH, cur_x, cur_y);
        }

        m_painter.setBrush(Qt::white);

        auto status = m_node_info.getStatus(m_cur_node);

        switch (status) {
            case NodeStatus::SOLVED: {
                m_painter.setBrush(colors::green);
                drawDiamond(m_painter, cur_x, cur_y, false);
            } break;
            case NodeStatus::FAILED: {
                m_painter.setBrush(colors::red);
                m_painter.drawRect(cur_x - HALF_FAILED_WIDTH, cur_y, FAILED_WIDTH, FAILED_WIDTH);
            } break;
            case NodeStatus::BRANCH: {
                auto selected = (m_user_data.getSelectedNode() == m_cur_node) ? true : false;
                drawBranchNode(m_painter, cur_x, cur_y, selected);
            } break;
            case NodeStatus::SKIPPED: {

            } break;
            default: {
                m_painter.setBrush(colors::white);
                m_painter.drawEllipse(cur_x - HALF_NODE_WIDTH, cur_y, NODE_WIDTH, NODE_WIDTH);
            } break;
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

}}
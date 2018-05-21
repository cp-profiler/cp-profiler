#include "drawing_cursor.hh"
#include "../../config.hh"
#include "../layout.hh"
#include "../node_info.hh"
#include "../node.hh"
#include "../shape.hh"
#include "../../user_data.hh"
#include "../../utils/tree_utils.hh"
#include "../traditional_view.hh"

#include "../node_drawing.hh"

#include "../node_tree.hh"

#include <QDebug>
#include <QPainter>

namespace cpprofiler { namespace tree {

    /// remove this copy
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
        const Layout& layout, const UserData& user_data, const VisualFlags& flags, QPainter& painter, QPoint start_pos, const QRect& clip)
    : NodeCursor(start, tree),
      m_node_tree(tree),
      m_layout(layout),
      user_data_(user_data),
      m_vis_flags(flags),
      m_painter(painter),
      clippingRect(clip)
    {
        cur_x = start_pos.x(); cur_y = start_pos.y();
    }

    static void drawTriangle(QPainter& painter, int x, int y, bool selected) {
        using namespace traditional;

        if (selected) {
            painter.setBrush(colors::gold);
        } else {
            painter.setBrush(colors::red);
        }

        QPointF points[3] = { QPointF(x, y),
            QPointF(x + HALF_COLLAPSED_WIDTH, y + COLLAPSED_DEPTH),
            QPointF(x - HALF_COLLAPSED_WIDTH, y + COLLAPSED_DEPTH)
        };

        painter.drawConvexPolygon(points, 3);
    }


    static void drawShape(QPainter& painter, int x, int y, NodeID nid, const Layout& layout) {
        using namespace traditional;

        auto old_pen = painter.pen();
        auto old_brush = painter.brush();

        painter.setBrush(QColor{0, 0, 0, 50});
        painter.setPen(Qt::NoPen);

        const auto& shape = layout.getShape(nid);

        const int height = shape.height();
        QPointF *points = new QPointF[height * 2];

        int l_x = x + shape[0].l;
        int r_x = x + shape[0].r;
        y = y + BRANCH_WIDTH / 2;

        points[0] = QPointF(l_x, y);
        points[height * 2 - 1] = QPointF(r_x, y);

        for (int i = 1; i <  height; i++){
            y += static_cast<double>(layout::dist_y);
            l_x = x + shape[i].l;
            r_x = x + shape[i].r;
            points[i] = QPointF(l_x, y);
            points[height * 2 - i - 1] = QPointF(r_x, y);
        }

        painter.drawConvexPolygon(points, shape.height() * 2);

        delete[] points;

        painter.setPen(old_pen);
        painter.setBrush(old_brush);
    }

    static void drawBoundingBox(QPainter& painter, int x, int y, NodeID nid, const Layout& layout) {
        auto bb = layout.getBoundingBox(nid);
        auto height = layout.getHeight(nid) * layout::dist_y;
        painter.drawRect(x + bb.left, y, bb.right - bb.left, height);
    }

    void DrawingCursor::processCurrentNode() {

        using namespace traditional;

        bool phantom_node = false;

        m_painter.setPen(QColor{Qt::black});

        if (m_cur_node != m_start_node) {

            auto parent_x = cur_x - m_layout.getOffset(m_cur_node);
            auto parent_y = cur_y - static_cast<double>(layout::dist_y);

            m_painter.drawLine(parent_x, parent_y + BRANCH_WIDTH, cur_x, cur_y);
        }

        auto status = m_node_info.getStatus(m_cur_node);

        /// NOTE: this should be consisten with the layout
        if (m_vis_flags.isLabelShown(m_cur_node)) {

            auto draw_left = !utils::is_right_most_child(m_node_tree, m_cur_node);
            // m_painter.setPen(QPen{Qt::black, 2});
            const auto& label = m_node_tree.getLabel(m_cur_node);

            auto fm = m_painter.fontMetrics();
            auto label_width = fm.width(label.c_str());

            {
                auto font = m_painter.font();
                font.setStyleHint(QFont::Monospace);
                m_painter.setFont(font);
            }

            int label_x;
            if (draw_left) {
                label_x = cur_x - HALF_MAX_NODE_W - label_width;
            } else {
                label_x = cur_x + HALF_MAX_NODE_W;
            }


            m_painter.drawText(QPoint{label_x, cur_y}, label.c_str());
        }

        if (m_vis_flags.isHighlighted(m_cur_node)) {
            drawShape(m_painter, cur_x, cur_y, m_cur_node, m_layout);
        }

        const auto sel_node = user_data_.getSelectedNode();
        const auto selected = (sel_node == m_cur_node) ? true : false;

        if (selected) {
            m_painter.setBrush(QColor{0, 0, 0, 20});

            drawBoundingBox(m_painter, cur_x, cur_y, m_cur_node, m_layout);

            drawShape(m_painter, cur_x, cur_y, m_cur_node, m_layout);
        }


        /// see if the node is hidden

        auto hidden = m_vis_flags.isHidden(m_cur_node);

        if (hidden) {

            if (status == NodeStatus::MERGED) {
                draw::big_pentagon(m_painter, cur_x, cur_y, selected);
                return;
            }

            /// completely failed
            drawTriangle(m_painter, cur_x, cur_y, selected);

            /// has open nodes

            /// has solutions
            return;
        }

        switch (status) {
            case NodeStatus::SOLVED: {
                draw::solution(m_painter, cur_x, cur_y, selected);
            } break;
            case NodeStatus::FAILED: {
                draw::failure(m_painter, cur_x, cur_y, selected);
            } break;
            case NodeStatus::BRANCH: {
                draw::branch(m_painter, cur_x, cur_y, selected);
            } break;
            case NodeStatus::SKIPPED: {
                draw::skipped(m_painter, cur_x, cur_y, selected);
            } break;
            case NodeStatus::MERGED: {
                draw::pentagon(m_painter, cur_x, cur_y, selected);
            } break;
            default: {
                draw::unexplored(m_painter, cur_x, cur_y, selected);
            } break;
        }

        if (user_data_.isBookmarked(m_cur_node)) {
            m_painter.setBrush(Qt::black);
            m_painter.drawEllipse(cur_x-10, cur_y, 10.0, 10.0);
        }

    }

    void DrawingCursor::moveUpwards() {
        cur_x -= m_layout.getOffset(m_cur_node);
        cur_y -= layout::dist_y;
        NodeCursor::moveUpwards();
    }

    void DrawingCursor::moveDownwards() {
        NodeCursor::moveDownwards();
        cur_x += m_layout.getOffset(m_cur_node);
        cur_y += layout::dist_y;
    }

    void DrawingCursor::moveSidewards() {
        cur_x -= m_layout.getOffset(m_cur_node);
        NodeCursor::moveSidewards();
        cur_x += m_layout.getOffset(m_cur_node);
    }

    bool DrawingCursor::mayMoveSidewards() {
        return NodeCursor::mayMoveSidewards();
    }

    bool DrawingCursor::mayMoveDownwards() {
        /// TODO: this should be about children?
        return NodeCursor::mayMoveDownwards() &&
               !m_vis_flags.isHidden(m_cur_node) &&
               m_layout.getLayoutDone(m_cur_node) &&
               !isClipped();
    }
    
    bool DrawingCursor::mayMoveUpwards() {
        return NodeCursor::mayMoveUpwards();
    }

    bool DrawingCursor::isClipped() {
        auto bb = m_layout.getBoundingBox(m_cur_node);

        if (
            (cur_x + bb.left > clippingRect.x() + clippingRect.width()) ||
            (cur_x + bb.right < clippingRect.x()) ||
            (cur_y > clippingRect.y() + clippingRect.height()) ||
            (cur_y + (m_layout.getHeight(m_cur_node) + 1) * layout::dist_y < clippingRect.y())
        ) {
            // qDebug() << "node clipped";
            return true;
        }

        return false;

    }

    void DrawingCursor::finalize() {
    }

}}
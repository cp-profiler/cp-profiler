
#include "traditional_view.hh"
#include "layout.hh"
#include "structure.hh"
#include "node_tree.hh"

#include <QPainter>
#include <QDebug>
#include <QScrollBar>

#include "cursors/nodevisitor.hh"
#include "cursors/drawing_cursor.hh"
#include "../utils/std_ext.hh"


namespace cpprofiler { namespace tree {


    void TreeScrollArea::paintEvent(QPaintEvent* event) {
        QPainter painter(this->viewport());
        painter.setRenderHint(QPainter::Antialiasing);


        {
            auto tree_width = 400;
            auto tree_height = 400;

            auto viewport_size = viewport()->size();

            auto h_page_step = viewport_size.width();
            auto v_page_step = viewport_size.height();

            horizontalScrollBar()->setRange(0, tree_width - h_page_step);
            horizontalScrollBar()->setPageStep(h_page_step);

            verticalScrollBar()->setRange(0, tree_height - v_page_step);
            verticalScrollBar()->setPageStep(v_page_step);

            horizontalScrollBar()->setSingleStep(Layout::min_dist_x);
            verticalScrollBar()->setSingleStep(Layout::dist_y);
        }

        QPen pen = painter.pen();
        pen.setWidth(2);
        painter.setPen(pen);

        auto x_off = horizontalScrollBar()->value();
        auto y_off = verticalScrollBar()->value();
        painter.translate(-x_off, -y_off);
        /// TODO: this need to aquire layout mutex

        auto half_w = viewport()->width() / 2;
        auto half_h = viewport()->height() / 2;

        painter.drawRect(x_off + 100, y_off + 100, 200, 200);

        // qDebug() << "x_off:" << x_off << "viewport width:" << viewport()->width();
        // qDebug() << "h page step: " << h_page_step;

        // helper::preOrder();

        DrawingCursor dc(m_tree.tree_structure().getRoot(), m_tree, m_layout, m_user_data, painter);
        PreorderNodeVisitor<DrawingCursor>(dc).run();


    }

    TreeScrollArea::TreeScrollArea(const NodeTree& tree, const UserData& user_data, const Layout& layout)
        : m_tree(tree), m_user_data(user_data), m_layout(layout) {

    }


}}

namespace cpprofiler { namespace tree {

TraditionalView::TraditionalView(const NodeTree& tree, const UserData& user_data)
: m_layout(utils::make_unique<Layout>()), m_scroll_area(tree, user_data, *m_layout)
{

    // m_layout.reset(new Layout);

    // m_scroll_area
    m_scroll_area.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_scroll_area.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    

    /// NOTE: for now, compute layout here

    const auto& layout_computer = m_layout->createComputer(tree.tree_structure());
    layout_computer->compute();

}

QWidget* TraditionalView::widget() {
    return &m_scroll_area;
}

TraditionalView::~TraditionalView() = default;

}}
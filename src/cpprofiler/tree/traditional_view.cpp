
#include "traditional_view.hh"
#include "layout.hh"
#include "structure.hh"

#include <QPainter>
#include <QDebug>

#include "cursors/nodevisitor.hh"
#include "cursors/drawing_cursor.hh"
#include "../utils/std_ext.hh"


namespace cpprofiler { namespace tree {


    void TreeScrollArea::paintEvent(QPaintEvent* event) {
        QPainter painter(this->viewport());
        painter.setRenderHint(QPainter::Antialiasing);

        QPen pen = painter.pen();
        pen.setWidth(1);
        painter.setPen(pen);


        /// TODO: this need to aquire layout mutex

        auto half_w = viewport()->width() / 2;
        auto half_h = viewport()->height() / 2;

        painter.drawRect(0,0, half_w - 1, half_h - 1);
        painter.drawRect(half_w, half_h, half_w, half_h);

        // helper::preOrder();

        DrawingCursor dc(m_tree.getRoot(), m_tree, m_layout, painter);
        PreorderNodeVisitor<DrawingCursor>(dc).run();
    }

    TreeScrollArea::TreeScrollArea(const Structure& tree, const Layout& layout)
        : m_tree(tree), m_layout(layout) {

    }


}}

namespace cpprofiler { namespace tree {

TraditionalView::TraditionalView(const Structure& tree)
: m_tree(tree), m_layout(utils::make_unique<Layout>()), m_scroll_area(tree, *m_layout)
{

    // m_layout.reset(new Layout);

    // m_scroll_area
    m_scroll_area.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_scroll_area.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    /// NOTE: for now, compute layout here

    const auto& layout_computer = m_layout->createComputer(m_tree);
    layout_computer->compute();
    // layout_computer->compute();

}

QWidget* TraditionalView::widget() {
    return &m_scroll_area;
}

TraditionalView::~TraditionalView() = default;

}}
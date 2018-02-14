
#include "traditional_view.hh"
#include "layout.hh"
#include "structure.hh"

#include <QPainter>
#include <QDebug>


namespace cpprofiler { namespace tree {


    void TreeScrollArea::paintEvent(QPaintEvent* event) {
        QPainter painter(this->viewport());
        painter.setRenderHint(QPainter::Antialiasing);

        // QPen pen = painter.pen();
        // pen.setWidth(1);
        // painter.setPen(pen);


        /// TODO: this need to aquire layout mutex

        painter.drawEllipse(50,50, 50, 50);

        helper::preOrder();
    }


}}

namespace cpprofiler { namespace tree {

TraditionalView::TraditionalView(const Structure& tree)
: m_tree(tree)
{

    m_layout.reset(new Layout);

    // m_scroll_area
    m_scroll_area.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_scroll_area.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    /// NOTE: for now, compute layout here

    const auto& layout_computer = m_layout->createComputer(m_tree);
    layout_computer->compute();

}

QWidget* TraditionalView::widget() {
    return &m_scroll_area;
}

TraditionalView::~TraditionalView() = default;

}}
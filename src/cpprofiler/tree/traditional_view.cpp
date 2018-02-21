
#include "traditional_view.hh"
#include "layout.hh"
#include "structure.hh"
#include "node_tree.hh"

#include <QPainter>
#include <QDebug>
#include <QScrollBar>
#include <QMouseEvent>

#include "cursors/nodevisitor.hh"
#include "cursors/drawing_cursor.hh"
#include "../utils/std_ext.hh"
#include "node_id.hh"
#include "shape.hh"
#include "../user_data.hh"
#include "layout_computer.hh"


namespace cpprofiler { namespace tree {

    constexpr int y_margin = 20;

    void TreeScrollArea::paintEvent(QPaintEvent* event) {
        QPainter painter(this->viewport());
        painter.setRenderHint(QPainter::Antialiasing);

        painter.scale(m_options.scale,m_options.scale);

        auto root_nid = m_tree.tree_structure().getRoot();

        {
            if (!m_layout.getLayoutDone(root_nid)) { return; }
            auto bb = m_layout.getBoundingBox(root_nid);
            auto tree_width = bb.right - bb.left;
            auto tree_height = m_layout.getDepth(root_nid) * Layout::dist_y;

            auto viewport_size = viewport()->size();

            auto h_page_step = viewport_size.width() / m_options.scale;
            auto v_page_step = viewport_size.height() / m_options.scale;


            /// set range in real pixels
            horizontalScrollBar()->setRange(0, tree_width - h_page_step);
            horizontalScrollBar()->setPageStep(h_page_step);

            verticalScrollBar()->setRange(0, tree_height + y_margin / m_options.scale - v_page_step);
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

        auto displayed_width = viewport()->size().width() / m_options.scale;

        auto start_pos = QPoint{m_options.x_off + displayed_width / 2, m_options.y_off + y_margin / m_options.scale};
        DrawingCursor dc(m_tree.tree_structure().getRoot(), m_tree, m_layout, m_user_data, m_node_flags, painter, start_pos);
        PreorderNodeVisitor<DrawingCursor>(dc).run();


    }

    // static bool containsCoordinates(NodeID nid, const Layout& layout, int x, int y) {
        
        
    //     layout
    // }

    NodeID TreeScrollArea::findNodeClicked(int x, int y) {
        /// calculate real x and y
        auto x_off = horizontalScrollBar()->value();
        auto y_off = verticalScrollBar()->value();

        x = x / m_options.scale + x_off;
        y = y / m_options.scale + y_off;

        // TODO

        auto& tree = m_tree.tree_structure();

        auto root = tree.getRoot();



        // auto order = tree::helper::postOrder(tree);

        // for (auto nid : order) {


        //     if (containsCoordinates(nid, m_layout, x, y)) {
        //         // m_user_data.setSelectedNode(nid);
        //         emit nodeClicked(nid);
        //         break;
        //     }
        // }



        return NodeID::NoNode;
    }

    void TreeScrollArea::mousePressEvent(QMouseEvent* me) {
        qDebug() << "mouse press";
        auto n = findNodeClicked(me->x(), me->y());

        // emit needsRedrawing();
        update();
    }

    TreeScrollArea::TreeScrollArea(const NodeTree& tree, const UserData& user_data, const Layout& layout, const DisplayState& options, const NodeFlags& nf)
        : m_tree(tree), m_user_data(user_data), m_layout(layout), m_options(options), m_node_flags(nf) {
    }


}}

namespace cpprofiler { namespace tree {

TraditionalView::TraditionalView(const NodeTree& tree)
: m_layout(utils::make_unique<Layout>()),
  m_user_data(utils::make_unique<UserData>()),
  m_scroll_area(tree, *m_user_data, *m_layout, m_options, m_flags),
  m_tree(tree.tree_structure()),
  m_layout_computer{utils::make_unique<LayoutComputer>(m_tree, *m_layout)}
{

    // m_scroll_area
    m_scroll_area.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_scroll_area.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    /// NOTE: for now, compute layout here
    m_layout_computer->compute();

    connect(&m_scroll_area, &TreeScrollArea::nodeClicked, this, &TraditionalView::nodeClicked);

    connect(this, &TraditionalView::needsRedrawing, [this]() {
        m_scroll_area.viewport()->update();
    });
}

TraditionalView::~TraditionalView() = default;

void TraditionalView::navDown() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    auto kids = m_tree.getNumberOfChildren(cur_nid);

    if (kids > 0) {
        auto first_kid = m_tree.getChild(cur_nid, 0);
        m_user_data->setSelectedNode(first_kid);
    }
    emit needsRedrawing();
}

void TraditionalView::navUp() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    auto pid = m_tree.getParent(cur_nid);

    if (pid != NodeID::NoNode) {
        m_user_data->setSelectedNode(pid);
    }

    emit needsRedrawing();
}

void TraditionalView::navLeft() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    auto pid = m_tree.getParent(cur_nid);
    if (pid == NodeID::NoNode) return;

    auto cur_alt = m_tree.getAlternative(cur_nid);

    auto kids = m_tree.getNumberOfChildren(pid);

    if (cur_alt > 0) {
        m_user_data->setSelectedNode(m_tree.getChild(pid, cur_alt - 1));
    }

    emit needsRedrawing();
}

void TraditionalView::navRight() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;
    auto pid = m_tree.getParent(cur_nid);

    if (pid == NodeID::NoNode) return;

    auto cur_alt = m_tree.getAlternative(cur_nid);

    auto kids = m_tree.getNumberOfChildren(pid);

    if (cur_alt + 1 < kids) {
        m_user_data->setSelectedNode(m_tree.getChild(pid, cur_alt + 1));
    }

    emit needsRedrawing();
}

void TraditionalView::toggleShowLabel() {
    auto cur_nid = m_user_data->getSelectedNode();

    auto val = !m_flags.get_label_shown(cur_nid);
    m_flags.set_label_shown(cur_nid, val);
    emit needsRedrawing();

    /// TODO: needs re-layout as well
}

QWidget* TraditionalView::widget() {
    return &m_scroll_area;
}

void TraditionalView::setScale(int scale) {
    m_options.scale = scale / 100.0f;
    m_scroll_area.viewport()->update();
}

void TraditionalView::centerNode(NodeID nid) {

}

void TraditionalView::selectNode(NodeID nid) {
    m_user_data->setSelectedNode(nid);
}

void TraditionalView::forceComputeLayout() {
    m_layout_computer->markAsOutdated();
    m_layout_computer->compute();
    emit needsRedrawing();
}

void TraditionalView::setLayoutOutdated() {
    /// TODO: change to re-layout on fewer nodes
    forceComputeLayout();
}


}}


namespace cpprofiler { namespace tree {


    void NodeFlags::set_label_shown(NodeID nid, bool val) {
        auto id = static_cast<int>(nid);
        if (m_label_shown.size() < id) {
            m_label_shown.resize(id + 1);
        }

        m_label_shown[id] = val;
    }

    bool NodeFlags::get_label_shown(NodeID nid) const {

        // return true; // TODO
        auto id = static_cast<int>(nid);
        if (m_label_shown.size() <= id) {
            return false;
        }

        return m_label_shown[id];
    }

}}
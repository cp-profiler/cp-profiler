
#include "traditional_view.hh"
#include "layout.hh"
#include "structure.hh"
#include "node_tree.hh"

#include <queue>
#include <thread>
#include <cmath>

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


#include "../utils/perf_helper.hh"




namespace cpprofiler { namespace tree {

    constexpr int y_margin = 20;

    static void drawGrid(QPainter& painter, QSize size) {

        const int cell_w = 100;
        const int cell_h = 100;

        {
            QPen pen = painter.pen();
            pen.setWidth(1);
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }

        const auto columns = std::ceil(static_cast<float>(size.width()) / cell_w);
        const auto rows = std::ceil(static_cast<float>(size.height()) / cell_h);

        for (auto row = 0; row < rows; ++row) {
            for (auto col = 0; col < columns; ++col) {
                auto x = col * cell_w;
                auto y = row * cell_h;
                painter.drawRect(x,y,cell_w,cell_w);
            }
        }
    }

    void TreeScrollArea::paintEvent(QPaintEvent* event) {
        QPainter painter(this->viewport());
        painter.setRenderHint(QPainter::Antialiasing);

        if (m_start_node == NodeID::NoNode) { return; }

        painter.scale(m_options.scale,m_options.scale);

        auto root_nid = m_tree.tree_structure().getRoot();

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

        {
            QPen pen = painter.pen();
            pen.setWidth(2);
            painter.setPen(pen);
        }
        

        auto x_off = horizontalScrollBar()->value();
        auto y_off = verticalScrollBar()->value();
        painter.translate(-x_off, -y_off);
        /// TODO: this need to aquire layout mutex

        auto half_w = viewport()->width() / 2;
        auto half_h = viewport()->height() / 2;

        auto displayed_width = static_cast<int>(viewport()->size().width() / m_options.scale);
        auto displayed_height = static_cast<int>(viewport()->size().height() / m_options.scale);

        // auto start_x = m_options.x_off + displayed_width / 2;

        if (displayed_width > tree_width) {
            m_options.root_x = (displayed_width - tree_width) / 2 - bb.left;
        } else {
            m_options.root_x = -bb.left;
        }

        m_options.root_y = y_margin / m_options.scale;
        auto start_pos = QPoint{m_options.root_x, m_options.root_y};

        const auto clip_margin = 0 / m_options.scale;

        QRect clip{ QPoint{x_off + clip_margin, y_off + clip_margin},
                    QSize{displayed_width - 2 * clip_margin, displayed_height - 2 * clip_margin} };

        painter.drawRect(clip);

        // drawGrid(painter, {std::max(tree_width, displayed_width), std::max(tree_height, displayed_height)});

        // perfHelper.begin("tree drawing");


        /// TODO: see if I can remove mutexes
        auto& tree_mutex = m_tree.tree_structure().getMutex();
        auto& layout_mutex = m_layout.getMutex();
        utils::MutexLocker t_lock(&tree_mutex);
        utils::MutexLocker l_lock(&layout_mutex);

        DrawingCursor dc(m_start_node, m_tree, m_layout, m_user_data, m_node_flags, painter, start_pos, clip);
        PreorderNodeVisitor<DrawingCursor>(dc).run();
        // perfHelper.end();

    }

    QPoint TreeScrollArea::getNodeCoordinate(NodeID nid) {
        auto& tree = m_tree.tree_structure();
        auto node = nid;
        auto pos = QPoint{m_options.root_x, m_options.root_y};

        while(node != tree.getRoot()) {
            pos += {m_layout.getOffset(node), Layout::dist_y};
            node = tree.getParent(node);
        }

        return pos;
    }

    static std::pair<int,int> getRealBB(NodeID nid, const Structure& tree, const Layout& layout, const DisplayState& ds) {

        auto bb = layout.getBoundingBox(nid);

        auto node = nid;
        while (node != tree.getRoot()) {
            bb.left += layout.getOffset(node);
            bb.right += layout.getOffset(node);
            node = tree.getParent(node);
        }

        bb.left += ds.root_x;
        bb.right += ds.root_x;

        return {bb.left, bb.right};
    }

    void TreeScrollArea::setScale(int val) {
        m_options.scale = val / 50.0f;
        viewport()->update();
    }

    NodeID TreeScrollArea::findNodeClicked(int x, int y) {

        using traditional::NODE_WIDTH;

        /// TODO: disable while constructing
        /// calculate real x and y
        auto x_off = horizontalScrollBar()->value();
        auto y_off = verticalScrollBar()->value();

        x = x / m_options.scale + x_off;
        y = y / m_options.scale + y_off;

        // qDebug() << "root x:" << m_options.root_x << "y:" << m_options.root_y;
        // qDebug() << "clicked:" << x << y;

        auto& tree = m_tree.tree_structure();

        std::queue<NodeID> queue;
        queue.push(tree.getRoot());

        while(!queue.empty()) {

            auto node = queue.front(); queue.pop();
            auto node_pos = getNodeCoordinate(node);
            auto node_pos_tl = node_pos - QPoint{NODE_WIDTH/2, 0};

            auto node_area = QRect(node_pos_tl, QSize{NODE_WIDTH, NODE_WIDTH});
            if (node_area.contains(x,y)) {
                return node;
            } else {

                auto kids = tree.getNumberOfChildren(node);

                for (auto i = 0; i < kids; ++i) {
                    auto kid = tree.getChild(node, i);

                    auto pair = getRealBB(kid, tree, m_layout, m_options);
                    if (pair.first <= x && pair.second >= x) {
                        queue.push(kid);
                    }
                }
            }
        }

        return NodeID::NoNode;
    }

    void TreeScrollArea::mousePressEvent(QMouseEvent* me) {
        auto n = findNodeClicked(me->x(), me->y());
        emit nodeClicked(n);
        update();
    }

    TreeScrollArea::TreeScrollArea(NodeID start, const NodeTree& tree, const UserData& user_data, const Layout& layout, const NodeFlags& nf)
        : m_start_node(start), m_tree(tree), m_user_data(user_data), m_layout(layout), m_node_flags(nf) {
    }

    void TreeScrollArea::centerX(int x) {
        auto viewport_size = viewport()->size();
        auto h_page_step = viewport_size.width() / m_options.scale;
        auto value = std::max(0, static_cast<int>(x - h_page_step/2));
        horizontalScrollBar()->setValue(value);
    }

    void TreeScrollArea::changeStartNode(NodeID nid) {
        m_start_node = nid;
    }


}}

namespace cpprofiler { namespace tree {

TraditionalView::TraditionalView(const NodeTree& tree)
: m_tree(tree.tree_structure()),
  m_user_data(utils::make_unique<UserData>()),
  m_layout(utils::make_unique<Layout>()),
  m_flags(utils::make_unique<NodeFlags>()),
  m_layout_computer(utils::make_unique<LayoutComputer>(tree, *m_layout, *m_flags))
{

    auto root_node = m_tree.getRoot();
    m_scroll_area.reset(new TreeScrollArea(root_node, tree, *m_user_data, *m_layout, *m_flags));

    std::cerr << "traditional view thread:" << std::this_thread::get_id() << std::endl;

    // m_scroll_area
    m_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    connect(m_scroll_area.get(), &TreeScrollArea::nodeClicked, this, &TraditionalView::selectNode);

    connect(this, &TraditionalView::needsRedrawing, [this]() {
        m_scroll_area->viewport()->update();
    });

    connect(&tree, &NodeTree::childrenStructureChanged, [this](NodeID nid) {
        // std::cerr << "dirty up thread:" << std::this_thread::get_id() << std::endl;
        m_layout_computer->dirtyUp(nid);
    });

    /// NOTE: for now, compute layout here
    if (tree.nodeCount() > 0) {
        m_layout_computer->compute();
    }

}

TraditionalView::~TraditionalView() = default;

NodeID TraditionalView::node() const {
    return m_user_data->getSelectedNode();
}

void TraditionalView::navDown() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    auto kids = m_tree.getNumberOfChildren(cur_nid);

    if (kids > 0) {
        auto first_kid = m_tree.getChild(cur_nid, 0);
        m_user_data->setSelectedNode(first_kid);
        centerCurrentNode();
    }

    emit needsRedrawing();
}

void TraditionalView::navUp() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    auto pid = m_tree.getParent(cur_nid);

    if (pid != NodeID::NoNode) {
        m_user_data->setSelectedNode(pid);
        centerCurrentNode();
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
        centerCurrentNode();
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
        centerCurrentNode();
    }

    emit needsRedrawing();
}

void TraditionalView::toggleShowLabel() {
    auto cur_nid = m_user_data->getSelectedNode();

    auto val = !m_flags->get_label_shown(cur_nid);
    m_flags->set_label_shown(cur_nid, val);
    emit needsRedrawing();

    /// TODO: needs re-layout as well
}

void TraditionalView::toggleHideFailed() {
    qDebug() << "TODO: hide failed";
}

void TraditionalView::toggleHighlighted() {
    auto nid = node();
    auto val = !m_flags->get_highlighted(nid);
    m_flags->set_highlighted(nid, val);
}

QWidget* TraditionalView::widget() {
    return m_scroll_area.get();
}

const Layout& TraditionalView::layout() const {
    return *m_layout;
}

void TraditionalView::setScale(int val) {
    m_scroll_area->setScale(val);
}

/// relative to the root
static int global_node_x_offset(const Structure& tree, const Layout& layout, NodeID nid) {
    auto x_off = 0;

    while (nid != NodeID::NoNode) {
        x_off += layout.getOffset(nid);
        nid = tree.getParent(nid);
    }

    return x_off;
}


void TraditionalView::centerNode(NodeID nid) {

    auto x_offset = global_node_x_offset(m_tree, *m_layout, nid);

    auto root_nid = m_tree.getRoot();
    auto bb = m_layout->getBoundingBox(root_nid);

    auto value = x_offset - bb.left;

    m_scroll_area->centerX(value);
}

void TraditionalView::centerCurrentNode() {
    auto cur_nid = m_user_data->getSelectedNode();
    centerNode(cur_nid);
}

void TraditionalView::selectNode(NodeID nid) {
    m_user_data->setSelectedNode(nid);
    emit needsRedrawing();
}

void TraditionalView::forceComputeLayout() {
    // perfHelper.begin("layout");
    m_layout_computer->markAsOutdated();
    m_layout_computer->compute();
    // perfHelper.end();
    emit needsRedrawing();
}

void TraditionalView::setLayoutOutdated() {
    /// TODO: change to re-layout on fewer nodes

    static int counter = 0;

    if (counter++ % 1 == 0) {
        forceComputeLayout();
    }
}

void TraditionalView::printNodeInfo() {
    auto cur_nid = m_user_data->getSelectedNode();

    if (cur_nid == NodeID::NoNode) return;

    qDebug() << "---- Node Info:" << cur_nid << "----";
    qDebug() << "offset:" << m_layout->getOffset(cur_nid);
    auto bb = m_layout->getBoundingBox(cur_nid);
    qDebug() << "bb:[" << bb.left << "," << bb.right << "]";
}

void TraditionalView::highlight_subtrees(const std::vector<NodeID>& nodes) {

    m_flags->unhighlight_all();

    for (auto nid : nodes) {
        m_flags->set_highlighted(nid, true);
    }

    emit needsRedrawing();
}


}}


namespace cpprofiler { namespace tree {

    void NodeFlags::ensure_id_exists(NodeID nid) {
        auto id = static_cast<int>(nid);
        if (m_label_shown.size() < id) {
            m_label_shown.resize(id + 1);
            m_node_hidden.resize(id + 1);
            m_shape_highlighted.resize(id + 1);
        }
    }


    void NodeFlags::set_label_shown(NodeID nid, bool val) {
        ensure_id_exists(nid);
        m_label_shown[nid] = val;
    }

    bool NodeFlags::get_label_shown(NodeID nid) const {

        return false; // TODO
        // ensure_id_exists(nid);

        return m_label_shown.at(nid);
    }

    void NodeFlags::set_hidden(NodeID nid, bool val) {
        // auto id = static_cast<int>(nid);
        // if ()
        /// TODO
    }

    bool NodeFlags::get_hidden(NodeID nid) const {
        return false; // TODO
    }

    void NodeFlags::set_highlighted(NodeID nid, bool val) {
        ensure_id_exists(nid);

        if (val) {
            m_highlighted_shapes.insert(nid);
        } else {
            m_highlighted_shapes.erase(nid);
        }
        m_shape_highlighted[nid] = val;
    }

    bool NodeFlags::get_highlighted(NodeID nid) const {

        if (m_shape_highlighted.size() <= nid) {
            return false;
        }
        return m_shape_highlighted[nid];
    }

    void NodeFlags::unhighlight_all() {
        for (auto nid : m_highlighted_shapes) {
            m_shape_highlighted[nid] = false;
        }

        m_highlighted_shapes.clear();
    }



}}
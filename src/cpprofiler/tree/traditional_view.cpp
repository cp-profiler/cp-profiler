#include "traditional_view.hh"
#include "tree_scroll_area.hh"
#include "layout.hh"
#include "structure.hh"
#include "node_tree.hh"

#include <queue>
#include <thread>
#include <cmath>

#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QScrollBar>
#include <QMouseEvent>

#include "cursors/nodevisitor.hh"
#include "cursors/hide_failed_cursor.hh"
#include "../utils/std_ext.hh"
#include "node_id.hh"
#include "shape.hh"
#include "../user_data.hh"
#include "layout_computer.hh"
#include "../config.hh"


#include "../utils/perf_helper.hh"
#include "../utils/tree_utils.hh"

namespace cpprofiler { namespace tree {

TraditionalView::TraditionalView(const NodeTree& tree)
: m_tree(tree),
  m_user_data(utils::make_unique<UserData>()),
  m_layout(utils::make_unique<Layout>()),
  m_vis_flags(utils::make_unique<VisualFlags>()),
  m_layout_computer(utils::make_unique<LayoutComputer>(tree, *m_layout, *m_vis_flags))
{

    auto root_node = m_tree.getRoot_safe();

    m_scroll_area.reset(new TreeScrollArea(root_node, m_tree, *m_user_data, *m_layout, *m_vis_flags));

    // auto painter = m_scroll_area.getLabelPainter();
    // m_layout_computer.setLabelPainter(painter);

    // std::cerr << "traditional view thread:" << std::this_thread::get_id() << std::endl;

    // m_scroll_area

    connect(m_scroll_area.get(), &TreeScrollArea::nodeClicked, this, &TraditionalView::selectNode);
    connect(m_scroll_area.get(), &TreeScrollArea::nodeDoubleClicked, this, &TraditionalView::handleDoubleClick);

    connect(this, &TraditionalView::needsRedrawing, this, &TraditionalView::redraw);

    connect(&tree, &NodeTree::childrenStructureChanged, [this](NodeID nid) {
        // std::cerr << "dirty up thread:" << std::this_thread::get_id() << std::endl;

        if (nid == NodeID::NoNode) { return; }

        auto nkids = m_tree.childrenCount(nid);

        for (auto i = 0; i < nkids; ++i) {
            auto kid = m_tree.getChild(nid, i);
            m_layout_computer->setDirty(kid);
        }
        m_layout_computer->dirtyUp(nid);
    });


    auto autoHideTimer = new QTimer(this);

    connect(autoHideTimer, &QTimer::timeout, this, &TraditionalView::autoUpdate);

    autoHideTimer->start(100);

    qDebug() << "TraditionalView() finished";

}

TraditionalView::~TraditionalView() = default;



/// Drawing needs the layout mutex only?
void TraditionalView::redraw() {

    m_scroll_area->viewport()->update();
}

NodeID TraditionalView::node() const {
    return m_user_data->getSelectedNode();
}

void TraditionalView::navRoot() {
    auto root = m_tree.getRoot();
    m_user_data->setSelectedNode(root);
    centerCurrentNode();
    emit needsRedrawing();
}

void TraditionalView::navDown() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    utils::DebugMutexLocker tree_lock(&m_tree.treeMutex());

    auto kids = m_tree.childrenCount(cur_nid);

    if (kids > 0) {
        auto first_kid = m_tree.getChild_safe(cur_nid, 0);
        m_user_data->setSelectedNode(first_kid);
        centerCurrentNode();
    }

    emit needsRedrawing();
}

void TraditionalView::navUp() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    utils::DebugMutexLocker tree_lock(&m_tree.treeMutex());

    auto pid = m_tree.getParent_safe(cur_nid);

    if (pid != NodeID::NoNode) {
        m_user_data->setSelectedNode(pid);
        centerCurrentNode();
    }
    emit needsRedrawing();
}

void TraditionalView::navLeft() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    utils::DebugMutexLocker tree_lock(&m_tree.treeMutex());

    auto pid = m_tree.getParent(cur_nid);
    if (pid == NodeID::NoNode) return;

    auto cur_alt = m_tree.getAlternative(cur_nid);

    auto kids = m_tree.childrenCount(pid);

    if (cur_alt > 0) {
        m_user_data->setSelectedNode(m_tree.getChild(pid, cur_alt - 1));
        centerCurrentNode();
    }

    emit needsRedrawing();
}

void TraditionalView::navRight() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    utils::DebugMutexLocker tree_lock(&m_tree.treeMutex());

    auto pid = m_tree.getParent_safe(cur_nid);

    if (pid == NodeID::NoNode) return;

    auto cur_alt = m_tree.getAlternative_safe(cur_nid);

    auto kids = m_tree.childrenCount(pid);

    if (cur_alt + 1 < kids) {
        m_user_data->setSelectedNode(m_tree.getChild_safe(pid, cur_alt + 1));
        centerCurrentNode();
    }

    emit needsRedrawing();
}

void TraditionalView::set_label_shown(NodeID nid, bool val) {
    m_vis_flags->set_label_shown(nid, val);
    m_layout_computer->dirtyUp(nid);
}

void TraditionalView::toggleShowLabel() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    auto val = !m_vis_flags->get_label_shown(cur_nid);
    set_label_shown(cur_nid, val);
    emit needsRedrawing();

    /// TODO: needs re-layout as well
}

void TraditionalView::showLabelsDown() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    auto val = !m_vis_flags->get_label_shown(cur_nid);

    m_tree.preOrderApply(cur_nid, [val, this](NodeID nid) {
        set_label_shown(nid, val);
    });


    setLayoutOutdated();
    emit needsRedrawing();
}

void TraditionalView::showLabelsUp() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    auto pid = m_tree.getParent_safe(cur_nid);

    /// if it is root, toggle for the root only
    if (pid == NodeID::NoNode) {
        toggleShowLabel();
        return;
    }

    auto val = !m_vis_flags->get_label_shown(pid);

    while (cur_nid != NodeID::NoNode) {
        set_label_shown(cur_nid, val);
        cur_nid = m_tree.getParent_safe(cur_nid);
    }

    setLayoutOutdated();
    emit needsRedrawing();
}

void TraditionalView::toggleHidden() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    // if leaf node -> do not hide
    if (m_tree.isLeaf(cur_nid)) return;

    auto val = !m_vis_flags->get_hidden(cur_nid);
    m_vis_flags->set_hidden(cur_nid, val);

    dirtyUp(cur_nid);

    setLayoutOutdated();
    emit needsRedrawing();
}

void TraditionalView::hideFailedAt(NodeID n) {
    HideFailedCursor hfc(n, m_tree, *m_vis_flags, *m_layout_computer);
    PostorderNodeVisitor<HideFailedCursor>(hfc).run();

    setLayoutOutdated();
    emit needsRedrawing();
}

void TraditionalView::hideFailed() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    hideFailedAt(cur_nid);
}

void TraditionalView::autoUpdate() {

    // {
    //     perfHelper.begin("hide failed");
    //     utils::DebugMutexLocker tree_lock(&m_tree.treeMutex());
    //     auto root = m_tree.getRoot();
    //     hideFailedAt(root);
    //     perfHelper.end();
    // }

    

    if (m_layout_stale) {
        computeLayout();
    }

}

void TraditionalView::handleDoubleClick() {

    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    auto status = m_tree.getStatus(cur_nid);

    if (status == NodeStatus::BRANCH) {
        unhideNode(cur_nid);
    } else if (status == NodeStatus::MERGED) {
        toggleCollapsePentagon(cur_nid);
    }
}

void TraditionalView::toggleCollapsePentagon(NodeID nid) {
    /// Use the same 'hideden' flag for now
    auto val = !m_vis_flags->get_hidden(nid);
    m_vis_flags->set_hidden(nid, val);
    dirtyUp(nid);
    setLayoutOutdated();
    emit needsRedrawing();
    
}

void TraditionalView::unhideNode(NodeID nid) {

    auto hidden = m_vis_flags->get_hidden(nid);
    if (hidden) {
        m_vis_flags->set_hidden(nid, false);
        dirtyUp(nid);
        setLayoutOutdated();
        emit needsRedrawing();
    }
}

void TraditionalView::unhideAll() {

    auto nid = m_user_data->getSelectedNode();
    if (nid == NodeID::NoNode) return;

    /// indicates if any change was made
    bool modified = false;

    const auto action = [&](NodeID n) {

        if (m_vis_flags->get_hidden(n)) {
            m_vis_flags->set_hidden(n, false);
            dirtyUp(n);
            modified = true;
        }

    };

    utils::apply_below(m_tree, nid, action);

    if (modified) {
        setLayoutOutdated();
        emit needsRedrawing();
    }
}

void TraditionalView::toggleHighlighted() {
    auto nid = m_user_data->getSelectedNode();
    if (nid == NodeID::NoNode) return;

    auto val = !m_vis_flags->get_highlighted(nid);
    m_vis_flags->set_highlighted(nid, val);

    emit needsRedrawing();
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
static int global_node_x_offset(const NodeTree& tree, const Layout& layout, NodeID nid) {
    auto x_off = 0;

    while (nid != NodeID::NoNode) {
        x_off += layout.getOffset(nid);
        nid = tree.getParent_safe(nid);
    }

    return x_off;
}


void TraditionalView::centerNode(NodeID nid) {

    auto x_offset = global_node_x_offset(m_tree, *m_layout, nid);

    auto root_nid = m_tree.getRoot_safe();
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
    computeLayout();
}

void TraditionalView::computeLayout() {
    qDebug() << "compute Layout";
    auto changed = m_layout_computer->compute();
    m_layout_stale = false;
    if (changed) {
        emit needsRedrawing();
    }
}

void TraditionalView::setLayoutOutdated() {
    qDebug() << "set layout stale";
    m_layout_stale = true;
}

void TraditionalView::dirtyUp(NodeID nid) {

    m_layout_computer->dirtyUp(nid);
}

void TraditionalView::dirtyCurrentNodeUp() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    dirtyUp(cur_nid);
}

void TraditionalView::printNodeInfo() {
    auto cur_nid = m_user_data->getSelectedNode();
    if (cur_nid == NodeID::NoNode) return;

    qDebug() << "---- Node Info:" << cur_nid << "----";
    qDebug() << "offset:" << m_layout->getOffset(cur_nid);
    auto bb = m_layout->getBoundingBox(cur_nid);
    qDebug() << "bb:[" << bb.left << "," << bb.right << "]";
    qDebug() << "dirty:" << m_layout->isDirty_unsafe(cur_nid);
    qDebug() << "hidden:" << m_vis_flags->get_hidden(cur_nid);
    qDebug() << "has solved kids:" << m_tree.hasSolvedChildren(cur_nid);
    qDebug() << "has open kids:" << m_tree.hasOpenChildren(cur_nid);
}

void TraditionalView::highlight_subtrees(const std::vector<NodeID>& nodes) {

    m_vis_flags->unhighlight_all();

    for (auto nid : nodes) {
        m_vis_flags->set_highlighted(nid, true);
    }

    emit needsRedrawing();
}


}}
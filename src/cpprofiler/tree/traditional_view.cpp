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
#include <QInputDialog>
#include <QLineEdit>

#include "cursors/nodevisitor.hh"
#include "cursors/hide_failed_cursor.hh"
#include "../utils/std_ext.hh"
#include "node_id.hh"
#include "shape.hh"
#include "../user_data.hh"
#include "../solver_data.hh"
#include "layout_computer.hh"
#include "../config.hh"

#include "../nogood_dialog.hh"

#include "../utils/perf_helper.hh"
#include "../utils/tree_utils.hh"

namespace cpprofiler { namespace tree {

TraditionalView::TraditionalView(const NodeTree& tree, UserData& ud, SolverData& sd)
: tree_(tree),
  user_data_(ud),
  solver_data_(sd),
  vis_flags_(utils::make_unique<VisualFlags>()),
  layout_(utils::make_unique<Layout>()),
  layout_computer_(utils::make_unique<LayoutComputer>(tree, *layout_, *vis_flags_))
{
    utils::DebugMutexLocker tree_lock(&tree_.treeMutex());

    scroll_area_.reset(new TreeScrollArea(tree_.getRoot(), tree_, user_data_, *layout_, *vis_flags_));

    // std::cerr << "traditional view thread:" << std::this_thread::get_id() << std::endl;

    connect(scroll_area_.get(), &TreeScrollArea::nodeClicked, this, &TraditionalView::setCurrentNode);
    connect(scroll_area_.get(), &TreeScrollArea::nodeDoubleClicked, this, &TraditionalView::handleDoubleClick);

    connect(this, &TraditionalView::needsRedrawing, this, &TraditionalView::redraw);

    connect(&tree, &NodeTree::childrenStructureChanged, [this](NodeID nid) {
        if (nid == NodeID::NoNode) { return; }
        layout_computer_->dirtyUp(nid);
    });


    auto autoHideTimer = new QTimer(this);

    connect(autoHideTimer, &QTimer::timeout, this, &TraditionalView::autoUpdate);

    autoHideTimer->start(30);

    print("TraditionalView()");

}

TraditionalView::~TraditionalView() = default;



void TraditionalView::redraw() {
    scroll_area_->viewport()->update();
}

NodeID TraditionalView::node() const {
    return user_data_.getSelectedNode();
}

void TraditionalView::setNode(NodeID nid) {
    user_data_.setSelectedNode(nid);
}

void TraditionalView::navRoot() {
    auto root = tree_.getRoot();
    setNode(root);
    centerCurrentNode(); /// TODO: this should be needed
    emit needsRedrawing();
}

void TraditionalView::navDown() {

    const auto nid = node();

    if (nid == NodeID::NoNode) return;

    utils::DebugMutexLocker tree_lock(&tree_.treeMutex());

    const auto kids = tree_.childrenCount(nid);

    if (kids == 0 || vis_flags_->isHidden(nid)) return;

    auto first_kid = tree_.getChild(nid, 0);
    setNode(first_kid);
    centerCurrentNode();
    emit needsRedrawing();
}

void TraditionalView::navDownAlt() {
    const auto nid = node();
    if (nid == NodeID::NoNode) return;

    utils::DebugMutexLocker tree_lock(&tree_.treeMutex());

    const auto kids = tree_.childrenCount(nid);

    if (kids == 0 || vis_flags_->isHidden(nid)) return;

    auto last_kid = tree_.getChild(nid, kids-1);
    setNode(last_kid);
    centerCurrentNode();
    emit needsRedrawing();
}

void TraditionalView::navUp() {
    auto nid = node();
    if (nid == NodeID::NoNode) return;

    utils::DebugMutexLocker tree_lock(&tree_.treeMutex());

    auto pid = tree_.getParent(nid);

    if (pid != NodeID::NoNode) {
        setNode(pid);
        centerCurrentNode();
    }
    emit needsRedrawing();
}

void TraditionalView::navLeft() {
    auto nid = node();
    if (nid == NodeID::NoNode) return;

    utils::DebugMutexLocker tree_lock(&tree_.treeMutex());

    auto pid = tree_.getParent(nid);
    if (pid == NodeID::NoNode) return;

    auto cur_alt = tree_.getAlternative(nid);

    if (cur_alt > 0) {
        auto kid = tree_.getChild(pid, cur_alt - 1);
        setNode(kid);
        centerCurrentNode();
    }

    emit needsRedrawing();
}

void TraditionalView::navRight() {
    /// lock mutex
    auto nid = node();
    if (nid == NodeID::NoNode) return;

    utils::DebugMutexLocker tree_lock(&tree_.treeMutex());

    auto pid = tree_.getParent(nid);

    if (pid == NodeID::NoNode) return;

    auto cur_alt = tree_.getAlternative(nid);

    auto kids = tree_.childrenCount(pid);

    if (cur_alt + 1 < kids) {
        const auto kid = tree_.getChild(pid, cur_alt + 1);
        setNode(kid);
        centerCurrentNode();
    }

    emit needsRedrawing();
}

void TraditionalView::setLabelShown(NodeID nid, bool val) {
    vis_flags_->setLabelShown(nid, val);
    layout_computer_->dirtyUp(nid);
}

void TraditionalView::toggleShowLabel() {
    auto nid = node();
    if (nid == NodeID::NoNode) return;

    auto val = !vis_flags_->isLabelShown(nid);
    setLabelShown(nid, val);
    emit needsRedrawing();

    /// TODO: needs re-layout as well
}

void TraditionalView::showLabelsDown() {
    auto nid = node();
    if (nid == NodeID::NoNode) return;

    auto val = !vis_flags_->isLabelShown(nid);

    utils::pre_order_apply(tree_, nid, [val, this](NodeID nid) {
        setLabelShown(nid, val);
    });

    setLayoutOutdated();
    emit needsRedrawing();
}

void TraditionalView::showLabelsUp() {
    auto nid = node();
    if (nid == NodeID::NoNode) return;

    auto pid = tree_.getParent(nid);

    /// if it is root, toggle for the root only
    if (pid == NodeID::NoNode) {
        toggleShowLabel();
        return;
    }

    auto val = !vis_flags_->isLabelShown(pid);

    while (nid != NodeID::NoNode) {
        setLabelShown(nid, val);
        nid = tree_.getParent(nid);
    }

    setLayoutOutdated();
    emit needsRedrawing();
}

static bool is_leaf(const NodeTree& nt, NodeID nid) {
    return nt.childrenCount(nid) == 0;
}

void TraditionalView::toggleHidden() {
    auto nid = node();
    if (nid == NodeID::NoNode) return;

    // do not hide leaf nodes
    if (is_leaf(tree_, nid)) return;

    auto val = !vis_flags_->isHidden(nid);
    vis_flags_->setHidden(nid, val);

    dirtyUp(nid);

    setLayoutOutdated();
    emit needsRedrawing();
}

void TraditionalView::hideFailedAt(NodeID n, bool onlyDirty) {
    /// Do nothing if there is no tree
    if (tree_.nodeCount() == 0) return;

    bool modified = false;

    HideFailedCursor hfc(n, tree_, *vis_flags_, *layout_computer_, onlyDirty, modified);
    PostorderNodeVisitor<HideFailedCursor>(hfc).run();

    if (modified) {
        setLayoutOutdated();
        emit needsRedrawing();
    }

}

void TraditionalView::hideFailed() {
    auto nid = node();
    if (nid == NodeID::NoNode) return;

    hideFailedAt(nid);
}

void TraditionalView::autoUpdate() {

    // {
    // //     perfHelper.begin("hide failed");
    //     utils::DebugMutexLocker tree_lock(&tree_.treeMutex());
    //     auto root = tree_.getRoot();
    //     hideFailedAt(root, true);
    // //     perfHelper.end();
    // }

    

    if (layout_stale_) {
        computeLayout();
    }

}

void TraditionalView::handleDoubleClick() {

    auto nid = node();
    if (nid == NodeID::NoNode) return;

    auto status = tree_.getStatus(nid);

    if (status == NodeStatus::BRANCH) {
        unhideNode(nid);
    } else if (status == NodeStatus::MERGED) {
        toggleCollapsePentagon(nid);
    }

    /// should this happen automatically whenever the layout is changed?
    centerCurrentNode();
}

void TraditionalView::toggleCollapsePentagon(NodeID nid) {
    /// Use the same 'hidden' flag for now
    auto val = !vis_flags_->isHidden(nid);
    vis_flags_->setHidden(nid, val);
    dirtyUp(nid);
    setLayoutOutdated();
    emit needsRedrawing();
    
}

void TraditionalView::unhideNode(NodeID nid) {

    auto hidden = vis_flags_->isHidden(nid);
    if (hidden) {
        vis_flags_->setHidden(nid, false);
        layout_->setLayoutDone(nid, false);
        dirtyUp(nid);
        setLayoutOutdated();
        emit needsRedrawing();
    }
}

void TraditionalView::bookmarkCurrentNode() {
    auto nid = node();

    if (!user_data_.isBookmarked(nid)) {
        /// Add bookmark
        bool accepted;
        auto text = QInputDialog::getText(nullptr, "Add bookmark", "Name:", QLineEdit::Normal, "", &accepted);
        if (!accepted) return;

        user_data_.setBookmark(nid, text.toStdString());
    } else {
        /// Remove bookmark
        user_data_.clearBookmark(nid);
    }

    emit needsRedrawing();
}

void TraditionalView::unhideAll() {

    perfHelper.begin("unhide_all");

    auto nid = node();
    if (nid == NodeID::NoNode) return;

    /// indicates if any change was made
    bool modified = false;

    const auto action = [&](NodeID n) {

        if (vis_flags_->isHidden(n)) {
            vis_flags_->setHidden(n, false);
            dirtyUp(n);
            modified = true;
        }

    };

    utils::apply_below(tree_, nid, action);

    if (modified) {
        setLayoutOutdated();
        emit needsRedrawing();
    }

    centerCurrentNode();

    perfHelper.end();
}

void TraditionalView::toggleHighlighted() {
    auto nid = node();
    if (nid == NodeID::NoNode) return;

    auto val = !vis_flags_->isHighlighted(nid);
    vis_flags_->setHighlighted(nid, val);

    emit needsRedrawing();
}

QWidget* TraditionalView::widget() {
    return scroll_area_.get();
}

const Layout& TraditionalView::layout() const {
    return *layout_;
}

void TraditionalView::setScale(int val) {
    scroll_area_->setScale(val);
}

/// relative to the root
static int global_node_x_offset(const NodeTree& tree, const Layout& layout, NodeID nid) {
    auto x_off = 0;

    while (nid != NodeID::NoNode) {
        x_off += layout.getOffset(nid);
        nid = tree.getParent(nid);
    }

    return x_off;
}

/// Does this need any locking?
void TraditionalView::centerNode(NodeID nid) {

    const auto x_offset = global_node_x_offset(tree_, *layout_, nid);

    const auto root_nid = tree_.getRoot();
    const auto bb = layout_->getBoundingBox(root_nid);

    const auto value_x = x_offset - bb.left;

    
    const auto depth = utils::calculate_depth(tree_, nid);
    const auto value_y = depth * layout::dist_y;

    scroll_area_->centerPoint(value_x, value_y);
}

void TraditionalView::centerCurrentNode() {
    centerNode(node());
}

void TraditionalView::setCurrentNode(NodeID nid) {
    user_data_.setSelectedNode(nid);
    emit needsRedrawing();
}

void TraditionalView::forceComputeLayout() {
    computeLayout();
}

void TraditionalView::computeLayout() {

    static int counter = 0;
    debug("layout") << "compute Layout:" << ++counter << "\n";
    auto changed = layout_computer_->compute();
    layout_stale_ = false;
    if (changed) {
        emit needsRedrawing();
    }
}

void TraditionalView::setLayoutOutdated() {
    debug("layout") << "set layout stale\n";
    layout_stale_ = true;
}

void TraditionalView::dirtyUp(NodeID nid) {
    layout_computer_->dirtyUp(nid);
}

void TraditionalView::dirtyCurrentNodeUp() {
    const auto nid = node();
    if (nid == NodeID::NoNode) return;

    dirtyUp(nid);
}

void TraditionalView::printNodeInfo() {
    const auto nid = node();
    if (nid == NodeID::NoNode) return;

    print("--- Node Info: {} ----", nid);
    print("offset: {}", layout_->getOffset(nid));
    auto bb = layout_->getBoundingBox(nid);
    print("bb:[{},{}]", bb.left, bb.right);
    print("dirty: {}", layout_->isDirty(nid));
    print("hidden: {}", vis_flags_->isHidden(nid));
    print("has solved kids: {}, ", tree_.hasSolvedChildren(nid));
    print("has open kids: {}", tree_.hasOpenChildren(nid));
    print("nogood: {}", tree_.getNogood(nid).get());
    print("alt: {}", tree_.getAlternative(nid));
}

void TraditionalView::highlightSubtrees(const std::vector<NodeID>& nodes) {

    vis_flags_->unhighlightAll();

    for (auto nid : nodes) {
        vis_flags_->setHighlighted(nid, true);
    }

    emit needsRedrawing();
}

void TraditionalView::showNogoods() const {

    if (!solver_data_.hasNogoods()) return;

    const auto cur_nid = node();
    if (cur_nid == NodeID::NoNode) return;

    const auto nodes = utils::nodes_below(tree_, cur_nid);

    auto ng_dialog = new NogoodDialog(tree_, nodes);
    ng_dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(ng_dialog, &NogoodDialog::nogoodClicked, [this](NodeID nid) {
        const_cast<TraditionalView*>(this)->setCurrentNode(nid);
        const_cast<TraditionalView*>(this)->centerCurrentNode();
    });

    ng_dialog->show();
}


}}
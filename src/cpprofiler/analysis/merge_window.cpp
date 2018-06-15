#include "merge_window.hh"

#include "../tree/traditional_view.hh"
#include "merging/pentagon_list_widget.hh"
#include "pentagon_counter.hpp"
#include "../user_data.hh"
#include "../solver_data.hh"
#include "../execution.hh"

#include "nogood_analysis_dialog.hh"

#include <QGridLayout>
#include <QWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QCheckBox>
#include <cmath>

namespace cpprofiler
{
namespace analysis
{

MergeWindow::MergeWindow(Execution &ex_l, Execution &ex_r, std::shared_ptr<tree::NodeTree> nt, std::shared_ptr<MergeResult> res)
    : ex_l_(ex_l), ex_r_(ex_r), nt_(nt), merge_result_(res)
{

    initOrigLocations();

    user_data_.reset(new UserData);
    solver_data_.reset(new SolverData);
    view_.reset(new tree::TraditionalView(*nt_, *user_data_, *solver_data_));

    view_->setScale(50);

    auto layout = new QGridLayout();

    resize(500 + pent_config::VIEW_WIDTH, 700);

    pentagon_bar = new PentagonCounter(this);
    statusBar()->addPermanentWidget(pentagon_bar);

    // auto dummy_result = new MergeResult{
    //     {NodeID{0}, 10, 20}
    //    ,{NodeID{1}, 20, 30}
    //    ,{NodeID{2}, 30, 40}
    //    ,{NodeID{4}, 160, 270}
    //    ,{NodeID{4}, 1160, 2270}
    //    ,{NodeID{3}, 40, 50}
    //    ,{NodeID{4}, 60, 70}
    // };

    // pent_list = new PentagonListWidget(this, *dummy_result);
    pent_list = new PentagonListWidget(this, *merge_result_);

    connect(pent_list, &PentagonListWidget::pentagonClicked, view_.get(), &tree::TraditionalView::setCurrentNode);
    // connect(pent_list, &PentagonListWidget::pentagonClicked, view_.get(), &tree::TraditionalView::setAndCenterNode);

    auto sort_cb = new QCheckBox("sorted", this);
    sort_cb->setChecked(true);
    connect(sort_cb, &QCheckBox::stateChanged, pent_list, &PentagonListWidget::handleSortCB);

    layout->addWidget(pent_list, 1, 0, 1, 1, Qt::AlignLeft);
    layout->addWidget(sort_cb, 0, 0, 1, 1, Qt::AlignLeft);

    {
        auto widget = new QWidget();
        setCentralWidget(widget);
        widget->setLayout(layout);
        layout->addWidget(view_->widget(), 0, 1, 2, 1);
    }

    auto menuBar = new QMenuBar(0);
// Don't add the menu bar on Mac OS X
#ifndef Q_WS_MAC
    /// is this needed???
    setMenuBar(menuBar);
#endif

    connect(nt_.get(), &tree::NodeTree::structureUpdated,
            view_.get(), &tree::TraditionalView::setLayoutOutdated);

    {
        auto nodeMenu = menuBar->addMenu("&Node");

        auto centerNode = new QAction{"Center current node", this};
        centerNode->setShortcut(QKeySequence("C"));
        nodeMenu->addAction(centerNode);
        connect(centerNode, &QAction::triggered, view_.get(), &tree::TraditionalView::centerCurrentNode);

        auto navRoot = new QAction{"Go to the root", this};
        navRoot->setShortcut(QKeySequence("R"));
        nodeMenu->addAction(navRoot);
        connect(navRoot, &QAction::triggered, view_.get(), &tree::TraditionalView::navRoot);

        auto navDown = new QAction{"Go down the tree", this};
        navDown->setShortcut(QKeySequence("Down"));
        nodeMenu->addAction(navDown);
        connect(navDown, &QAction::triggered, view_.get(), &tree::TraditionalView::navDown);

        auto navUp = new QAction{"Go up the tree", this};
        navUp->setShortcut(QKeySequence("Up"));
        nodeMenu->addAction(navUp);
        connect(navUp, &QAction::triggered, view_.get(), &tree::TraditionalView::navUp);

        auto navLeft = new QAction{"Go left the tree", this};
        navLeft->setShortcut(QKeySequence("Left"));
        nodeMenu->addAction(navLeft);
        connect(navLeft, &QAction::triggered, view_.get(), &tree::TraditionalView::navLeft);

        auto navRight = new QAction{"Go right the tree", this};
        navRight->setShortcut(QKeySequence("Right"));
        nodeMenu->addAction(navRight);
        connect(navRight, &QAction::triggered, view_.get(), &tree::TraditionalView::navRight);

        auto toggleShowLabel = new QAction{"Show labels down", this};
        toggleShowLabel->setShortcut(QKeySequence("L"));
        nodeMenu->addAction(toggleShowLabel);
        connect(toggleShowLabel, &QAction::triggered, view_.get(), &tree::TraditionalView::showLabelsDown);

        auto toggleShowLabelsUp = new QAction{"Show labels down", this};
        toggleShowLabelsUp->setShortcut(QKeySequence("Shift+L"));
        nodeMenu->addAction(toggleShowLabelsUp);
        connect(toggleShowLabelsUp, &QAction::triggered, view_.get(), &tree::TraditionalView::showLabelsUp);

        auto hideFailed = new QAction{"Hide failed", this};
        hideFailed->setShortcut(QKeySequence("F"));
        nodeMenu->addAction(hideFailed);
        connect(hideFailed, &QAction::triggered, view_.get(), &tree::TraditionalView::hideFailed);

        auto unhideAll = new QAction{"Unhide all", this};
        unhideAll->setShortcut(QKeySequence("U"));
        nodeMenu->addAction(unhideAll);
        connect(unhideAll, &QAction::triggered, view_.get(), &tree::TraditionalView::unhideAll);

        auto toggleHighlighted = new QAction{"Toggle highlight subtree", this};
        toggleHighlighted->setShortcut(QKeySequence("H"));
        nodeMenu->addAction(toggleHighlighted);
        connect(toggleHighlighted, &QAction::triggered, view_.get(), &tree::TraditionalView::toggleHighlighted);
    }

    {
        auto debugMenu = menuBar->addMenu("&Debug");

        auto computeLayout = new QAction{"Compute layout", this};
        debugMenu->addAction(computeLayout);
        connect(computeLayout, &QAction::triggered, view_.get(), &tree::TraditionalView::computeLayout);

        auto updateView = new QAction{"Update view", this};
        debugMenu->addAction(updateView);
        connect(updateView, &QAction::triggered, view_.get(), &tree::TraditionalView::needsRedrawing);
    }

    {
        auto analysisMenu = menuBar->addMenu("&Analysis");

        auto ngAnalysisAction = new QAction{"Nogood analysis", this};
        analysisMenu->addAction(ngAnalysisAction);
        connect(ngAnalysisAction, &QAction::triggered, this, &MergeWindow::runNogoodAnalysis);
    }

    pentagon_bar->update(merge_result_->size());
    pent_list->updateScene();
}

MergeWindow::~MergeWindow() = default;

/// Find original ids for nodes under a pentagon
static void linkLocationsPentagon(NodeID n_m,
                                  const tree::NodeTree &nt_m,
                                  NodeID n,
                                  const tree::NodeTree &nt,
                                  std::vector<OriginalLoc> &locs)
{
    std::stack<NodeID> stack_m; /// stack for nodes of the merged tree
    std::stack<NodeID> stack;   /// stack for nodes of the original tree

    if (n_m == NodeID::NoNode && n == NodeID::NoNode)
        return;

    stack_m.push(n_m);
    stack.push(n);

    while (!stack_m.empty())
    {
        auto node_m = stack_m.top();
        stack_m.pop();
        auto node = stack.top();
        stack.pop();

        locs[n_m] = {n};

        /// The trees at this point must have the same strucutre
        for (auto alt = nt_m.childrenCount(node_m) - 1; alt >= 0; --alt)
        {
            stack_m.push(nt_m.getChild(node_m, alt));
            stack.push(nt.getChild(node, alt));
        }
    }
}

void MergeWindow::initOrigLocations()
{
    // TODO: Perform DFS traversal of three trees in lockstep assigning ids
    orig_locations_.resize(nt_->nodeCount());

    std::stack<NodeID> stack_m; /// stack for nodes of the merged tree
    std::stack<NodeID> stack_l; /// stack for nodes of the left tree
    std::stack<NodeID> stack_r; /// stack for nodes of the right tree

    auto &nt_l = ex_l_.tree(); /// left tree
    auto &nt_r = ex_r_.tree(); /// right tree

    stack_m.push(nt_->getRoot());
    stack_l.push(nt_l.getRoot());
    stack_r.push(nt_r.getRoot());

    while (!stack_m.empty())
    {
        auto n = stack_m.top();
        stack_m.pop();
        auto n_l = stack_l.top();
        stack_l.pop();
        auto n_r = stack_r.top();
        stack_r.pop();

        if (nt_->getStatus(n) == tree::NodeStatus::MERGED)
        {
            auto left_child = nt_->getChild(n, 0);
            auto right_child = nt_->getChild(n, 1);
            linkLocationsPentagon(left_child, *nt_, n_l, nt_l, orig_locations_);
            linkLocationsPentagon(right_child, *nt_, n_r, nt_r, orig_locations_);
            continue;
        }

        orig_locations_[n] = {n_l}; /// arbitrarily link to the node on the left tree

        /// Note: the tree above merged nodes must have the same structure
        for (auto alt = nt_->childrenCount(n) - 1; alt >= 0; --alt)
        {
            stack_m.push(nt_->getChild(n, alt));
            stack_l.push(nt_l.getChild(n_l, alt));
            stack_r.push(nt_r.getChild(n_r, alt));
        }
    }
}

tree::NodeTree &MergeWindow::getTree()
{
    return *nt_;
}

MergeResult &MergeWindow::mergeResult()
{
    return *merge_result_;
}

namespace ng_analysis
{

struct ReductionStats
{
    int total_red; /// total reduction by a nogood
    int count;     /// number of times a nogood contributed to a 1-n pentagon
};

class ResultBuilder
{

    using ResType = std::unordered_map<NogoodID, ReductionStats>;

    /// Accumulate nogood contributions here
    ResType ng_items;

  public:
    ResultBuilder() {}

    /// Account for search reduction of one 1-n pentagon
    void addPentagonData(
        const std::vector<NogoodID> &nogoods, // responsible nogoods
        int red)                              // node reduction (n-1)
    {
        /// reduction attributed to each nogood
        const auto rel_red = std::ceil((float)red / nogoods.size());

        for (auto ng : nogoods)
        {
            if (ng_items.find(ng) == ng_items.end())
            {
                /// never seen this nogood before
                ng_items.insert({ng, {0, 0}});
            }

            auto &ng_stats = ng_items.at(ng);
            ng_stats.count++;
            ng_stats.total_red += rel_red;
        }
    }

    const ResType &result() const { return ng_items; }
};

} // namespace ng_analysis

void MergeWindow::runNogoodAnalysis() const
{

    print("merge result size: {}", merge_result_->size());

    const auto &tree_l = ex_l_.tree();

    ng_analysis::ResultBuilder res_builder;

    for (auto &item : *merge_result_)
    {

        if (item.size_l != 1)
            continue;

        /// See what nogoods contribute to the nogood at item.nid

        /// get left child
        auto kid_l = nt_->getChild(item.pen_nid, 0);
        auto orig_id = orig_locations_[kid_l].nid;

        const auto &ng = tree_l.getNogood(orig_id);

        const auto &sd = tree_l.solver_data();

        print("nogood: {}", ng.get());

        /// get contributing nogoods:

        const auto *nogoods = sd.getContribNogoods(orig_id);

        if (nogoods)
        {
            print("contributing nogoods: {}", *nogoods);
            res_builder.addPentagonData(*nogoods, item.size_r - item.size_l);
        }
    }

    /// construct ng analysis data in the format required by ng dialog
    std::vector<NgAnalysisItem> nga_data;
    nga_data.reserve(res_builder.result().size());

    for (auto item : res_builder.result())
    {
        const NogoodID id = item.first;
        const auto &ng_str = tree_l.getNogood(id);

        nga_data.push_back({id, ng_str, item.second.total_red, item.second.count});
    }

    auto ng_window = new NogoodAnalysisDialog(nga_data);
    ng_window->setAttribute(Qt::WA_DeleteOnClose);

    connect(ng_window, &NogoodAnalysisDialog::nogoodClicked, [this](NodeID nid) {
        const_cast<tree::TraditionalView *>(view_.get())->setCurrentNode(nid);
        const_cast<tree::TraditionalView *>(view_.get())->centerCurrentNode();
    });

    ng_window->show();
}

// NodeID MergeWindow::findOriginalId(NodeID nid) const
// {
//     auto iter = nid;

//     /// For now assume the node comes from left tree
//     bool left_tree = true;

//     while (iter != NodeID::NoNode)
//     {
//         const auto status = nt_.getStatus(iter);
//         if (status == tree::NodeStatus::MERGED)
//         {
//             print("found pentagon node: {}", iter);

//             for ()
//         }

//         iter = nt_.getParent(iter);
//     }
// }

} // namespace analysis
} // namespace cpprofiler
#include "merge_window.hh"

#include "../tree/traditional_view.hh"
#include "merging/pentagon_list_widget.hh"
#include "pentagon_counter.hpp"
#include "../user_data.hh"
#include "../solver_data.hh"

#include <QGridLayout>
#include <QWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QCheckBox>
#include <QDialog>

namespace cpprofiler
{
namespace analysis
{

MergeWindow::MergeWindow(std::shared_ptr<tree::NodeTree> nt, std::shared_ptr<MergeResult> res)
    : nt_(nt), merge_result_(res)
{
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
        connect(computeLayout, &QAction::triggered, view_.get(), &tree::TraditionalView::forceComputeLayout);

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

tree::NodeTree &MergeWindow::getTree()
{
    return *nt_;
}

MergeResult &MergeWindow::mergeResult()
{
    return *merge_result_;
}

void MergeWindow::runNogoodAnalysis() const
{

    print("merge result size: {}", merge_result_->size());

    for (auto &item : *merge_result_)
    {
        if (item.size_l == 1)
        {

            /// See if item.nid is associated with any nogood

            /// get left child
            auto kid_l = nt_->getChild(item.pen_nid, 0);

            print("1-many pentagon: {}", item.pen_nid);

            // findOriginalId(kid_l);

            /// Should merge window have its own solver data (nogoods?)
            // solver_data_
        }
    }

    print("ng analysis done");
    // auto nga_dialog = new QDialog();

    // nga_dialog->show();
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
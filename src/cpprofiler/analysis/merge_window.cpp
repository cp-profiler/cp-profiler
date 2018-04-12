#include "merge_window.hh"

#include "../tree/traditional_view.hh"
#include "merging/pentagon_list_widget.hh"
#include "pentagon_counter.hpp"

#include <QGridLayout>
#include <QWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QCheckBox>


namespace cpprofiler { namespace analysis {

    MergeWindow::MergeWindow() {

        m_view.reset(new tree::TraditionalView(m_nt));

        m_view->setScale(50);

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
        pent_list = new PentagonListWidget(this, m_merge_result);

        connect(pent_list, &PentagonListWidget::pentagonClicked, m_view.get(), &tree::TraditionalView::selectNode);

        auto sort_cb = new QCheckBox("sorted", this);
        sort_cb->setChecked(true);
        connect(sort_cb, &QCheckBox::stateChanged, pent_list, &PentagonListWidget::handleSortCB);

        layout->addWidget(pent_list, 1, 0, 1, 1, Qt::AlignLeft);
        layout->addWidget(sort_cb, 0, 0, 1, 1, Qt::AlignLeft);

        {
            auto widget = new QWidget();
            setCentralWidget(widget);
            widget->setLayout(layout);
            layout->addWidget(m_view->widget(), 0, 1, 2, 1);
        }

        auto menuBar = new QMenuBar(0);
        // Don't add the menu bar on Mac OS X
        #ifndef Q_WS_MAC
            /// is this needed???
          setMenuBar(menuBar);
        #endif

        connect(&m_nt, &tree::NodeTree::structureUpdated,
                    m_view.get(), &tree::TraditionalView::setLayoutOutdated);


        {
            auto nodeMenu = menuBar->addMenu("&Node");

            auto navDown = new QAction{"Go down the tree", this};
            navDown->setShortcut(QKeySequence("Down"));
            nodeMenu->addAction(navDown);
            connect(navDown, &QAction::triggered, m_view.get(), &tree::TraditionalView::navDown);

            auto navUp = new QAction{"Go up the tree", this};
            navUp->setShortcut(QKeySequence("Up"));
            nodeMenu->addAction(navUp);
            connect(navUp, &QAction::triggered, m_view.get(), &tree::TraditionalView::navUp);

            auto navLeft = new QAction{"Go left the tree", this};
            navLeft->setShortcut(QKeySequence("Left"));
            nodeMenu->addAction(navLeft);
            connect(navLeft, &QAction::triggered, m_view.get(), &tree::TraditionalView::navLeft);

            auto navRight = new QAction{"Go right the tree", this};
            navRight->setShortcut(QKeySequence("Right"));
            nodeMenu->addAction(navRight);
            connect(navRight, &QAction::triggered, m_view.get(), &tree::TraditionalView::navRight);

            auto toggleShowLabel = new QAction{"Show labels down", this};
            toggleShowLabel->setShortcut(QKeySequence("L"));
            nodeMenu->addAction(toggleShowLabel);
            connect(toggleShowLabel, &QAction::triggered, m_view.get(), &tree::TraditionalView::showLabelsDown);

            auto toggleShowLabelsUp = new QAction{"Show labels down", this};
            toggleShowLabelsUp->setShortcut(QKeySequence("Shift+L"));
            nodeMenu->addAction(toggleShowLabelsUp);
            connect(toggleShowLabelsUp, &QAction::triggered, m_view.get(), &tree::TraditionalView::showLabelsUp);

            auto hideFailed = new QAction{"Hide failed", this};
            hideFailed->setShortcut(QKeySequence("F"));
            nodeMenu->addAction(hideFailed);
            connect(hideFailed, &QAction::triggered, m_view.get(), &tree::TraditionalView::hideFailed);

            auto unhideAll = new QAction{"Unhide all", this};
            unhideAll->setShortcut(QKeySequence("U"));
            nodeMenu->addAction(unhideAll);
            connect(unhideAll, &QAction::triggered, m_view.get(), &tree::TraditionalView::unhideAll);

            auto toggleHighlighted = new QAction{"Toggle highlight subtree", this};
            toggleHighlighted->setShortcut(QKeySequence("H"));
            nodeMenu->addAction(toggleHighlighted);
            connect(toggleHighlighted, &QAction::triggered, m_view.get(), &tree::TraditionalView::toggleHighlighted);
        }


        {
            auto debugMenu = menuBar->addMenu("&Debug");

            auto computeLayout = new QAction{"Compute layout", this};
            debugMenu->addAction(computeLayout);
            connect(computeLayout, &QAction::triggered, m_view.get(), &tree::TraditionalView::forceComputeLayout);

            auto updateView = new QAction{"Update view", this};
            debugMenu->addAction(updateView);
            connect(updateView, &QAction::triggered, m_view.get(), &tree::TraditionalView::needsRedrawing);
        }
    }


    MergeWindow::~MergeWindow() = default;

    tree::NodeTree& MergeWindow::getTree() {
        return m_nt;
    }

    MergeResult& MergeWindow::mergeResult() {
        return m_merge_result;
    }

    void MergeWindow::finalize() {
        pentagon_bar->update(m_merge_result.size());
        pent_list->updateScene();
    }



}}
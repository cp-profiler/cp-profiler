#include "merge_window.hh"
#include "../tree/traditional_view.hh"

#include <QGridLayout>
#include <QWidget>
#include <QMenuBar>

namespace cpprofiler { namespace analysis {

    MergeWindow::MergeWindow() {

        m_view.reset(new tree::TraditionalView(m_nt));

        m_view->setScale(50);

        auto layout = new QGridLayout();

        resize(500, 700);

        // setWindowOpacity(0.9);

        {
            auto widget = new QWidget();
            setCentralWidget(widget);
            widget->setLayout(layout);
            layout->addWidget(m_view->widget(), 0, 0, 2, 1);
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

            auto toggleHidden = new QAction{"Toggle hide failed", this};
            toggleHidden->setShortcut(QKeySequence("F"));
            nodeMenu->addAction(toggleHidden);
            connect(toggleHidden, &QAction::triggered, m_view.get(), &tree::TraditionalView::toggleHidden);

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



}}
#include "execution_window.hh"

#include "tree/traditional_view.hh"
#include "execution.hh"
#include "user_data.hh"

#include <QGridLayout>
#include <QSlider>
#include <QDebug>
#include <QMenuBar>

namespace cpprofiler {

    ExecutionWindow::ExecutionWindow(Execution& ex)
    : m_execution(ex)
    {
        auto& node_tree = ex.tree();
        m_traditional_view.reset(new tree::TraditionalView(node_tree));

        auto layout = new QGridLayout();

        resize(500,500);

        {
            auto widget = new QWidget();
            setCentralWidget(widget);
            widget->setLayout(layout);
            layout->addWidget(m_traditional_view->widget(), 0, 0, -1, 1);
        }

        {

            auto sb = new QSlider(Qt::Vertical);
            // sb->setObjectName("scaleBar");
            // sb->setMinimum(LayoutConfig::minScale);
            // sb->setMaximum(LayoutConfig::maxScale);

            
            sb->setMinimum(1);
            sb->setMaximum(100);

            constexpr int start_scale = 50;
            sb->setValue(start_scale);
            layout->addWidget(sb, 1, 1, Qt::AlignHCenter);

            m_traditional_view->setScale(start_scale);

            connect(sb, &QSlider::valueChanged,
                m_traditional_view.get(), &tree::TraditionalView::setScale);
            

            // connect(m_traditional_view.get(), &tree::TraditionalView::nodeClicked,
            //     this, &ExecutionWindow::selectNode);

            connect(&m_execution.tree(), &tree::NodeTree::structureUpdated,
                    m_traditional_view.get(), &tree::TraditionalView::setLayoutOutdated);

        }

        {
            auto menuBar = new QMenuBar(0);
            // Don't add the menu bar on Mac OS X
            #ifndef Q_WS_MAC
                /// is this needed???
              setMenuBar(menuBar);
            #endif

            {
                auto nodeMenu = menuBar->addMenu("&Node");

                auto navDown = new QAction{"Go down the tree", this};
                navDown->setShortcut(QKeySequence("Down"));
                nodeMenu->addAction(navDown);
                connect(navDown, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::navDown);

                auto navUp = new QAction{"Go up the tree", this};
                navUp->setShortcut(QKeySequence("Up"));
                nodeMenu->addAction(navUp);
                connect(navUp, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::navUp);

                auto navLeft = new QAction{"Go left the tree", this};
                navLeft->setShortcut(QKeySequence("Left"));
                nodeMenu->addAction(navLeft);
                connect(navLeft, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::navLeft);

                auto navRight = new QAction{"Go right the tree", this};
                navRight->setShortcut(QKeySequence("Right"));
                nodeMenu->addAction(navRight);
                connect(navRight, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::navRight);

                auto toggleShowLabel = new QAction{"Show label", this};
                toggleShowLabel->setShortcut(QKeySequence("L"));
                nodeMenu->addAction(toggleShowLabel);
                connect(toggleShowLabel, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::toggleShowLabel);
            }


            {
                auto debugMenu = menuBar->addMenu("&Debug");

                auto computeLayout = new QAction{"Compute layout", this};

                debugMenu->addAction(computeLayout);

                connect(computeLayout, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::forceComputeLayout);
            }

        }
    }

    ExecutionWindow::~ExecutionWindow() = default;

    tree::TraditionalView& ExecutionWindow::traditional_view() {
        return *m_traditional_view;
    }

}
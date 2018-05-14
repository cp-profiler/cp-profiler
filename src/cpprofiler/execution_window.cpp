#include "execution_window.hh"

#include "tree/traditional_view.hh"
#include "execution.hh"
#include "user_data.hh"

#include <QGridLayout>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QSlider>
#include <QDebug>
#include <QMenuBar>
#include <QTextEdit>
#include <QFile>
#include <QTimer>
#include <QStatusBar>

#include "tree/node_tree.hh"

#include "analysis/similar_subtree_window.hh"

#include "stats_bar.hpp"

namespace cpprofiler {

    ExecutionWindow::ExecutionWindow(Execution& ex)
    : m_execution(ex)
    {
        auto& node_tree = ex.tree();
        m_traditional_view.reset(new tree::TraditionalView(node_tree));

        auto layout = new QGridLayout();

        statusBar()->showMessage("Ready");

        auto stats_bar = new NodeStatsBar(this, node_tree.node_stats());
        statusBar()->addPermanentWidget(stats_bar);

        resize(500,700);

        {
            auto widget = new QWidget();
            setCentralWidget(widget);
            widget->setLayout(layout);
            layout->addWidget(m_traditional_view->widget(), 0, 0, 2, 1);
        }

        {

            auto sb = new QSlider(Qt::Vertical);

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

            {
                auto statsUpdateTimer = new QTimer(this);
                connect(statsUpdateTimer, &QTimer::timeout, stats_bar, &NodeStatsBar::update);
                statsUpdateTimer->start(16);
            }

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

                auto centerNode = new QAction{"Center current node", this};
                centerNode->setShortcut(QKeySequence("C"));
                nodeMenu->addAction(centerNode);
                connect(centerNode, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::centerCurrentNode);

                auto toggleShowLabel = new QAction{"Show labels down", this};
                toggleShowLabel->setShortcut(QKeySequence("L"));
                nodeMenu->addAction(toggleShowLabel);
                connect(toggleShowLabel, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::showLabelsDown);

                auto toggleShowLabelsUp = new QAction{"Show labels down", this};
                toggleShowLabelsUp->setShortcut(QKeySequence("Shift+L"));
                nodeMenu->addAction(toggleShowLabelsUp);
                connect(toggleShowLabelsUp, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::showLabelsUp);

                auto hideFailed = new QAction{"Hide failed", this};
                hideFailed->setShortcut(QKeySequence("F"));
                nodeMenu->addAction(hideFailed);
                connect(hideFailed, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::hideFailed);

                auto unhideAll = new QAction{"Unhide all", this};
                unhideAll->setShortcut(QKeySequence("U"));
                nodeMenu->addAction(unhideAll);
                connect(unhideAll, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::unhideAll);

                auto toggleHighlighted = new QAction{"Toggle highlight subtree", this};
                toggleHighlighted->setShortcut(QKeySequence("H"));
                nodeMenu->addAction(toggleHighlighted);
                connect(toggleHighlighted, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::toggleHighlighted);

                auto bookmarkNode = new  QAction{"Add/remove bookmark", this};
                bookmarkNode->setShortcut(QKeySequence("Shift+B"));
                nodeMenu->addAction(bookmarkNode);
                connect(bookmarkNode, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::bookmarkCurrentNode);

            }

            {

                auto navMenu = menuBar->addMenu("Na&vigation");

                auto navRoot = new QAction{"Go to the root", this};
                navRoot->setShortcut(QKeySequence("R"));
                navMenu->addAction(navRoot);
                connect(navRoot, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::navRoot);

                auto navDown = new QAction{"Go to the left-most child", this};
                navDown->setShortcut(QKeySequence("Down"));
                navMenu->addAction(navDown);
                connect(navDown, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::navDown);

                auto navDownAlt = new QAction{"Go to the right-most child", this};
                navDownAlt->setShortcut(QKeySequence("Alt+Down"));
                navMenu->addAction(navDownAlt);
                connect(navDownAlt, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::navDownAlt);

                auto navUp = new QAction{"Go up", this};
                navUp->setShortcut(QKeySequence("Up"));
                navMenu->addAction(navUp);
                connect(navUp, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::navUp);

                auto navLeft = new QAction{"Go left", this};
                navLeft->setShortcut(QKeySequence("Left"));
                navMenu->addAction(navLeft);
                connect(navLeft, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::navLeft);

                auto navRight = new QAction{"Go right", this};
                navRight->setShortcut(QKeySequence("Right"));
                navMenu->addAction(navRight);
                connect(navRight, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::navRight);
            }


            {
                auto debugMenu = menuBar->addMenu("Debu&g");

                auto computeLayout = new QAction{"Compute layout", this};
                debugMenu->addAction(computeLayout);
                connect(computeLayout, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::forceComputeLayout);

                auto dirtyNodesUp = new QAction{"Dirty Nodes Up", this};
                debugMenu->addAction(dirtyNodesUp);
                connect(dirtyNodesUp, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::dirtyCurrentNodeUp);

                auto getNodeInfo = new QAction{"Print node info", this};
                getNodeInfo->setShortcut(QKeySequence("I"));
                debugMenu->addAction(getNodeInfo);
                connect(getNodeInfo, &QAction::triggered, m_traditional_view.get(), &tree::TraditionalView::printNodeInfo);
            }

            {
                auto dataMenu = menuBar->addMenu("&Data");

                auto showBookmarks = new QAction{"Show bookmarks", this};
                dataMenu->addAction(showBookmarks);
                connect(showBookmarks, &QAction::triggered, this, &ExecutionWindow::showBookmarks);
            }

            {
                auto analysisMenu = menuBar->addMenu("&Analyses");
                auto similarSubtree = new QAction{"Similar Subtrees", this};
                similarSubtree->setShortcut(QKeySequence("Shift+S"));
                analysisMenu->addAction(similarSubtree);

                const auto& tree_layout = m_traditional_view->layout();

                connect(similarSubtree, &QAction::triggered, [this, &ex, &tree_layout]() {
                    auto ssw = new analysis::SimilarSubtreeWindow(this, ex.tree(), tree_layout);
                    ssw->show();

                    connect(ssw, &analysis::SimilarSubtreeWindow::should_be_highlighted,
                        m_traditional_view.get(), &tree::TraditionalView::highlightSubtrees);
                });

                auto saveSearch = new QAction{"Save Search (for replaying)", this};
                analysisMenu->addAction(saveSearch);
                connect(saveSearch, &QAction::triggered, [this]() { emit needToSaveSearch(&m_execution); });
            }

            // auto debugText = new QTextEdit{this};
            // // debugText->setHeight(200);
            // debugText->setReadOnly(true);

            // layout->addWidget(debugText, 2, 0);

        }
    }

    ExecutionWindow::~ExecutionWindow() = default;

    tree::TraditionalView& ExecutionWindow::traditional_view() {
        return *m_traditional_view;
    }

    void ExecutionWindow::showBookmarks() const {

        /// is this deleted?
        auto b_window = new QDialog();
        b_window->setAttribute(Qt::WA_DeleteOnClose);

        auto lo = new QVBoxLayout(b_window);

        auto bm_table = new QTableView();

        auto bm_model = new QStandardItemModel(0, 2);
        QStringList headers{"NodeID", "Bookmark Text"};
        bm_model->setHorizontalHeaderLabels(headers);
        bm_table->horizontalHeader()->setStretchLastSection(true);

        bm_table->setModel(bm_model);

        auto nid_item = new QStandardItem("-1");
        auto text_item = new QStandardItem("Test Bookmark");
        bm_model->appendRow({nid_item, text_item});

        lo->addWidget(bm_table);

        b_window->show();

        // QTableView

    }

    static void writeToFile(const QString& path, const QString& str) {
      QFile file(path);

      if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);

        out << str;

      } else {
        qDebug() << "could not open the file: " << path;
      }
    }


    void ExecutionWindow::print_log(const std::string& str) {
        writeToFile("debug.log", str.c_str());
    }

}
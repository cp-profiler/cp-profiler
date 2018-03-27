#include "merge_window.hh"
#include "../tree/traditional_view.hh"

#include <QGridLayout>
#include <QWidget>

namespace cpprofiler { namespace analysis {

    MergeWindow::MergeWindow() {

        m_view.reset(new tree::TraditionalView(m_nt));

        auto layout = new QGridLayout();

        resize(500, 700);

        {
            auto widget = new QWidget();
            setCentralWidget(widget);
            widget->setLayout(layout);
            layout->addWidget(m_view->widget(), 0, 0, 2, 1);
        }
    }


    MergeWindow::~MergeWindow() = default;

    tree::NodeTree& MergeWindow::getTree() {
        return m_nt;
    }



}}
#include "execution_window.hh"

#include "tree/traditional_view.hh"
#include "execution.hh"

#include <QGridLayout>

namespace cpprofiler {

    ExecutionWindow::ExecutionWindow(const Execution& ex)
    : m_execution(ex)
    {
        auto& node_tree = ex.tree_structure();
        m_traditional_view.reset(new tree::TraditionalView(node_tree));

        auto layout = new QGridLayout();

        {
            auto widget = new QWidget();
            setCentralWidget(widget);
            widget->setLayout(layout);
        }

        resize(500,500);

        layout->addWidget(m_traditional_view->widget(), 0, 0, -1, 1);
    }

    ExecutionWindow::~ExecutionWindow() = default;

}
#include "execution_window.hh"

#include "tree/traditional_view.hh"
#include "execution.hh"

#include <QGridLayout>
#include <QSlider>

namespace cpprofiler {

    ExecutionWindow::ExecutionWindow(const Execution& ex)
    : m_execution(ex)
    {
        auto& node_tree = ex.tree();
        m_traditional_view.reset(new tree::TraditionalView(node_tree, ex.user_data()));

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
            
            sb->setMinimum(0);
            sb->setMaximum(100);

            sb->setValue(50);
            layout->addWidget(sb, 1, 1, Qt::AlignHCenter);
        }
    }

    ExecutionWindow::~ExecutionWindow() = default;

}
#ifndef CPPROFILER_EXECUTION_WINDOW
#define CPPROFILER_EXECUTION_WINDOW

#include <memory>
#include <QMainWindow>

namespace cpprofiler {

namespace tree {
    class TraditionalView;
}

class Execution;

class ExecutionWindow : public QMainWindow {

    const Execution& m_execution;
    std::unique_ptr<tree::TraditionalView> m_traditional_view;

public:

    ExecutionWindow(const Execution& ex);
    ~ExecutionWindow();

};


}



#endif
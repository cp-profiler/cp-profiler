#ifndef CPPROFILER_EXECUTION_WINDOW
#define CPPROFILER_EXECUTION_WINDOW

#include <memory>
#include <QMainWindow>

#include "tree/node_id.hh"

namespace cpprofiler {

namespace tree {
    class TraditionalView;
}

class Execution;


class ExecutionWindow : public QMainWindow {
    Q_OBJECT

    Execution& m_execution;
    std::unique_ptr<tree::TraditionalView> m_traditional_view;

public:

    tree::TraditionalView& traditional_view();

    ExecutionWindow(Execution& ex);
    ~ExecutionWindow();

public slots:

    void print_log(const std::string& str);

signals:

    void needToSaveSearch(Execution* e);

};


}



#endif
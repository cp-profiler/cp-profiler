#ifndef CPPROFILER_EXECUTION_LIST
#define CPPROFILER_EXECUTION_LIST


#include <QStandardItemModel>
#include <QTreeView>
#include <memory>

namespace cpprofiler {

class Execution;

class ExecutionItem : public QStandardItem {
    Execution& m_execution;

public:
    ExecutionItem(Execution& e);
};

class ExecutionList {

    QTreeView m_tree_view;
    QStandardItemModel m_execution_tree_model;


public:

    ExecutionList();

    QWidget* getWidget() { return &m_tree_view; }

    void addExecution(Execution& e);
};


}

#endif
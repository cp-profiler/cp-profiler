#include "execution_list.hh"
#include "execution.hh"


namespace cpprofiler {

    ExecutionItem::ExecutionItem(Execution& e)
        : QStandardItem(e.name().c_str()), m_execution(e) {}
}

namespace cpprofiler {

    ExecutionList::ExecutionList() {
        m_tree_view.setHeaderHidden(true);
        m_tree_view.setModel(&m_execution_tree_model);
    }

    void ExecutionList::addExecution(Execution& e) {
        m_execution_tree_model.appendRow(new ExecutionItem{e});
    }

}
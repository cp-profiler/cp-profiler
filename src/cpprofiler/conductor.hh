#ifndef CPPROFILER_CONDUCTOR_HH
#define CPPROFILER_CONDUCTOR_HH

#include <QMainWindow>
#include <QStandardItemModel>
#include <memory>
#include <unordered_map>

namespace cpprofiler {

class TcpServer;
class Execution;
class ExecutionList;
class ExecutionWindow;

class Conductor : public QMainWindow {
Q_OBJECT

public:

    Conductor();

    ~Conductor();

    void addNewExecution(Execution* e);

private:

    // void getSelectedExecutions

    void showTraditionalView(const Execution* e);

    static constexpr quint16 DEFAULT_PORT = 6565;

    std::unique_ptr<TcpServer> m_server;
    std::vector<std::shared_ptr<Execution>> m_executions;

    std::unique_ptr<ExecutionList> m_execution_list;

    std::unordered_map<const Execution*,
        std::unique_ptr<ExecutionWindow>> m_execution_windows;
};


}


#endif
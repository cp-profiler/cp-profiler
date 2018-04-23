#ifndef CPPROFILER_CONDUCTOR_HH
#define CPPROFILER_CONDUCTOR_HH

#include <QMainWindow>
#include <QStandardItemModel>
#include <memory>
#include <map>
#include <unordered_map>

#include "settings.hh"
#include "options.hh"

namespace cpprofiler {

class TcpServer;
class Execution;
class ExecutionList;
class ExecutionWindow;
class ReceiverThread;
class TreeBuilder;

class Conductor : public QMainWindow {
Q_OBJECT

public:

    Conductor(Options opt);

    ~Conductor();

    void handleStart(ReceiverThread* receiver, const std::string& ex_name, int ex_id, bool restarts);

    Execution* addNewExecution(const std::string& ex_name, int ex_id = -1, bool restarts = false);

    void showTraditionalView(Execution* e);

    void mergeTrees(Execution* e1, Execution* e2);

    ExecutionWindow& getExecutionWindow(Execution* e);


signals:

    void readyForBuilding(Execution* e);

private:

    void readSettings();

    // void getSelectedExecutions

    static constexpr quint16 DEFAULT_PORT = 6565;

    Settings m_settings;

    std::unique_ptr<TcpServer> m_server;
    // std::vector<std::shared_ptr<Execution>> m_executions;

    Options m_options;

    /// a map from execution id to an execution
    std::unordered_map<int, std::shared_ptr<Execution>> m_executions;

    /// a map from exec_id to its builder
    std::unordered_map<int, TreeBuilder*> m_builders;

    std::unique_ptr<ExecutionList> m_execution_list;

    std::unordered_map<const Execution*,
        std::unique_ptr<ExecutionWindow>> m_execution_windows;
};


}


#endif
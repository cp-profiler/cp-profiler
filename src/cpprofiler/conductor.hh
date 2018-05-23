#ifndef CPPROFILER_CONDUCTOR_HH
#define CPPROFILER_CONDUCTOR_HH

#include <QMainWindow>
#include <QStandardItemModel>
#include <map>
#include <memory>
#include <unordered_map>

#include "options.hh"
#include "settings.hh"

namespace cpprofiler
{

namespace analysis
{
class MergeWindow;
}

class TcpServer;
class Execution;
class ExecutionList;
class ExecutionWindow;
class ReceiverThread;
class TreeBuilder;
class NameMap;

using ExecID = int;

struct ExecMeta
{
    std::string group_name;
    std::string ex_name;
    std::shared_ptr<NameMap> name_map;
};

class Conductor : public QMainWindow
{
    Q_OBJECT

  public:
    Conductor(Options opt);

    ~Conductor();

    void handleStart(ReceiverThread *receiver, const std::string &ex_name,
                     int ex_id, bool restarts);

    Execution *addNewExecution(const std::string &ex_name, int ex_id = 0,
                               bool restarts = false);

    /// Add existing execution to the displayed list
    void addNewExecution(std::shared_ptr<Execution> ex);

    void showTraditionalView(Execution *e);

    void mergeTrees(Execution *e1, Execution *e2);

    void saveSearch(Execution *e, const char *path) const;
    void saveSearch(Execution *e) const;

    void runNogoodAnalysis(Execution *e1, Execution *e2);

    ExecutionWindow &getExecutionWindow(Execution *e);

    int getListenPort() const;

    int getNextExecId() const;

    void setMetaData(int exec_id, const std::string &group_name,
                     const std::string &execution_name,
                     std::shared_ptr<NameMap> nm);

  signals:

    void readyForBuilding(Execution *e);

  private:
    void saveExecution(Execution *e);

    void readSettings();

    void onExecutionDone(Execution *e) const;

    // void getSelectedExecutions

    static constexpr quint16 DEFAULT_PORT = 6565;

    Settings settings_;

    /// Port number opened for solvers to connect to
    quint16 listen_port_;

    std::unique_ptr<TcpServer> server_;

    Options options_;

    /// a map from execution id to an execution
    std::unordered_map<int, std::shared_ptr<Execution>> executions_;

    /// a map from exec_id to its builder
    std::unordered_map<int, TreeBuilder *> builders_;

    std::unique_ptr<ExecutionList> execution_list_;

    std::unordered_map<ExecID, ExecMeta> exec_meta_;

    std::unordered_map<const Execution *, std::unique_ptr<ExecutionWindow>>
        execution_windows_;
};

} // namespace cpprofiler

#endif
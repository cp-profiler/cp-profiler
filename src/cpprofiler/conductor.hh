#ifndef CPPROFILER_CONDUCTOR_HH
#define CPPROFILER_CONDUCTOR_HH

#include <QMainWindow>
#include <memory>

namespace cpprofiler {

class TcpServer;
class Execution;

class Conductor : public QMainWindow {
Q_OBJECT

public:

    Conductor();

    ~Conductor();

    void addNewExecution(Execution* e);

private:

    static constexpr quint16 DEFAULT_PORT = 6565;

    std::unique_ptr<TcpServer> m_server;
    std::vector<std::shared_ptr<Execution>> m_executions;

};


}


#endif
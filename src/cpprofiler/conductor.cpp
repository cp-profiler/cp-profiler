#include "conductor.hh"
#include "tcp_server.hh"
#include "receiver_thread.hh"
#include <iostream>
#include <thread>

#include "execution.hh"


namespace cpprofiler {

    Conductor::Conductor() {

        setWindowTitle("CP-Profiler");

        m_server.reset(new TcpServer([this](intptr_t socketDesc) {

            // TODO(maxim): interpret the data, see if new execution is needed

            std::cerr << "callback, handle: " << socketDesc << std::endl;

            auto receiver = new ReceiverThread(socketDesc);
            connect(receiver, &QThread::finished, receiver, &QObject::deleteLater);

            connect(receiver, &ReceiverThread::newExecution,
                this, &Conductor::addNewExecution);

            receiver->start();

        }));

        quint16 listen_port = DEFAULT_PORT;

        // See if the default port is available
        auto res = m_server->listen(QHostAddress::Any, listen_port);
        if (!res) {
            // If not, try any port
            m_server->listen(QHostAddress::Any, 0);
            listen_port = m_server->serverPort();
        }

        std::cerr << "Ready to listen on: " << listen_port << std::endl;

    }

    Conductor::~Conductor(){}

    void Conductor::addNewExecution(Execution* e) {

        m_executions.push_back(std::shared_ptr<Execution>(e));

        std::cerr << "conductor new execution thread" << std::this_thread::get_id() << std::endl;
        std::cerr << "TODO: create new execution\n";
    }

}
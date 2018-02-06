#include "receiver_thread.hh"

#include "receiver_worker.hh"
#include "execution.hh"

#include <iostream>
#include <thread>
#include <chrono>
#include <QTcpSocket>

namespace cpprofiler {

    class Message;

    ReceiverThread::ReceiverThread(intptr_t socket_desc)
        : m_socket_desc(socket_desc)
    {
        std::cerr << "socket descriptor: " << socket_desc << std::endl;
    }

    void ReceiverThread::run() {

        QTcpSocket socket;

        m_worker.reset(new ReceiverWorker{socket});

        /// propagate the signal further upwards;
        /// blocking connection is used to ensure that the execution is created
        /// before any further message is processed
        connect(m_worker.get(), &ReceiverWorker::newExecution,
            this, &ReceiverThread::newExecution, Qt::BlockingQueuedConnection);

        connect(m_worker.get(), &ReceiverWorker::newNode,
            this, &ReceiverThread::newNode);

        connect(m_worker.get(), &ReceiverWorker::doneReceiving,
            this, &ReceiverThread::doneReceiving);

        auto res = socket.setSocketDescriptor(m_socket_desc);

        if (!res) {
            std::cerr << "invalid socket descriptor\n";
            this->quit(); return;
        }

        connect(&socket, &QTcpSocket::readyRead, m_worker.get(), &ReceiverWorker::doRead);

        connect(&socket, &QTcpSocket::disconnected, [this]() {
            this->quit();
        });

        exec();
    }

    ReceiverThread::~ReceiverThread() {
        std::cerr << "~ReceiverThread\n";
    }
}
#include "conductor.hh"
#include "tcp_server.hh"
#include "receiver_thread.hh"
#include <iostream>
#include <thread>
#include <QTreeView>
#include <QGridLayout>
#include <QPushButton>
#include <QDebug>
#include "cpp-integration/message.hpp"

#include "execution.hh"
#include "builder_thread.hh"
#include "execution_list.hh"
#include "execution_window.hh"

#include "utils/std_ext.hh"

namespace cpprofiler {

    Conductor::Conductor() {

        setWindowTitle("CP-Profiler");

        auto layout = new QGridLayout();

        {
            auto window = new QWidget();
            setCentralWidget(window);
            window->setLayout(layout);
        }

        m_execution_list.reset(new ExecutionList);
        layout->addWidget(m_execution_list->getWidget());

        auto showButton = new QPushButton("Show Tree");
        layout->addWidget(showButton);
        connect(showButton, &QPushButton::clicked, [this]() {

            for (auto execution : m_execution_list->getSelected()) {
                showTraditionalView(execution);
            }

        });

        m_server.reset(new TcpServer([this](intptr_t socketDesc) {

            // TODO(maxim): interpret the data, see if new execution is needed

            std::cerr << "callback, handle: " << socketDesc << std::endl;

            {
                auto receiver = new ReceiverThread(socketDesc);
                connect(receiver, &QThread::finished, receiver, &QObject::deleteLater);

                connect(receiver, &ReceiverThread::newExecution,
                    this, &Conductor::addNewExecution);

                auto builder = new BuilderThread();
                connect(receiver, &QThread::finished, receiver, &QObject::deleteLater);
                connect(builder, &QThread::finished, builder, &QObject::deleteLater);

                connect(receiver, &ReceiverThread::newExecution,
                    builder, &BuilderThread::startBuilding);

                connect(receiver, &ReceiverThread::newNode,
                    builder, &BuilderThread::handleNode);

                connect(receiver, &ReceiverThread::doneReceiving,
                    builder, &BuilderThread::finishBuilding);

                receiver->start();
                builder->start();
            }

            {
                
            }

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

    Conductor::~Conductor(){

        std::cerr << "~Conductor\n";
    }

    void Conductor::addNewExecution(Execution* e) {

        std::cerr << "new execution created\n";

        m_executions.push_back(std::shared_ptr<Execution>(e));
        m_execution_list->addExecution(*e);

    }

}


namespace cpprofiler {

    void Conductor::showTraditionalView(const Execution* e) {
        auto maybe_view = m_execution_windows.find(e);

        /// create new one if doesn't already exist
        if (maybe_view == m_execution_windows.end()) {
            // m_execution_windows[e] = std::unique_ptr<ExecutionWindow>(new ExecutionWindow);
            m_execution_windows[e] = utils::make_unique<ExecutionWindow>(*e);
        }

        m_execution_windows.at(e)->show();

    }
}
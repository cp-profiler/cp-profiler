#include "conductor.hh"
#include "tcp_server.hh"
#include "receiver_thread.hh"
#include <iostream>
#include <thread>
#include <QTreeView>
#include <QGridLayout>
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include "cpp-integration/message.hpp"

#include "execution.hh"
#include "tree_builder.hh"
#include "execution_list.hh"
#include "execution_window.hh"

#include "analysis/merge_window.hh"
#include "analysis/tree_merger.hh"

#include "utils/std_ext.hh"


#include <QProgressDialog>

namespace cpprofiler {

    Conductor::Conductor() {

        setWindowTitle("CP-Profiler");

        readSettings();

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
                auto receiver = new ReceiverThread(socketDesc, m_settings);
                connect(receiver, &QThread::finished, receiver, &QObject::deleteLater);
                connect(receiver, &ReceiverThread::newExecution,
                    this, &Conductor::addNewExecution);
                connect(receiver, &QThread::finished, receiver, &QObject::deleteLater);

                auto builderThread = new QThread();
                auto builder = new TreeBuilder();

                builder->moveToThread(builderThread);
                connect(builderThread, &QThread::finished, builderThread, &QObject::deleteLater);

                connect(receiver, &ReceiverThread::newExecution,
                    builder, &TreeBuilder::startBuilding);

                connect(receiver, &ReceiverThread::newNode,
                    builder, &TreeBuilder::handleNode);

                connect(receiver, &ReceiverThread::doneReceiving,
                    builder, &TreeBuilder::finishBuilding);

                receiver->start();
                builderThread->start();
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

        debug("memory") << "~Conductor\n";
    }

    void Conductor::addNewExecution(Execution* e) {

        m_executions.push_back(std::shared_ptr<Execution>(e));
        m_execution_list->addExecution(*e);

        showTraditionalView(e);

    }

    ExecutionWindow& Conductor::getExecutionWindow(Execution* e) {
        auto maybe_view = m_execution_windows.find(e);

        /// create new one if doesn't already exist
        if (maybe_view == m_execution_windows.end()) {

            m_execution_windows[e] = utils::make_unique<ExecutionWindow>(*e);
        }

        return *m_execution_windows.at(e);
    }

    void Conductor::mergeTrees(Execution* e1, Execution* e2) {

        /// create new tree

        // QProgressDialog dialog;

        auto window = new analysis::MergeWindow();

        auto& new_tree = window->getTree();
        auto& res = window->mergeResult();

        /// Note: TreeMerger will delete itself when finished
        auto merger = new analysis::TreeMerger(*e1, *e2, new_tree, res);

        connect(merger, &analysis::TreeMerger::finished,
                window, &analysis::MergeWindow::finalize);
        merger->start();

        window->show();

    }

    void Conductor::readSettings() {

        QFile file("settings.json");

        if (!file.exists()) {
            qDebug() << "settings.json not found";
            return;
        }

        file.open(QIODevice::ReadWrite | QIODevice::Text);

        auto data = file.readAll();

        auto json_doc = QJsonDocument::fromJson(data);

        if (json_doc.isEmpty()) {
            qDebug() << "settings.json is empty";
            return;
        }

        if (json_doc.isObject()) {

            auto json_obj = json_doc.object();

            m_settings.receiver_delay = json_obj["receiver_delay"].toInt();
        }

        qDebug() << "settings read";

    }

}


namespace cpprofiler {

    void Conductor::showTraditionalView(Execution* e) {

        auto& window = getExecutionWindow(e);

        window.show();

    }
}
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
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QApplication>
#include "../cpp-integration/message.hpp"

#include "execution.hh"
#include "tree_builder.hh"
#include "execution_list.hh"
#include "execution_window.hh"

#include "analysis/merge_window.hh"
#include "analysis/tree_merger.hh"

#include "utils/std_ext.hh"
#include "utils/tree_utils.hh"

#include <random>

#include <QProgressDialog>

#include "name_map.hh"
#include "db_handler.hh"

namespace cpprofiler
{

Conductor::Conductor(Options opt) : options_(opt)
{

    setWindowTitle("CP-Profiler");

    readSettings();

    auto layout = new QGridLayout();

    {
        auto window = new QWidget();
        setCentralWidget(window);
        window->setLayout(layout);
    }

    execution_list_.reset(new ExecutionList);
    layout->addWidget(execution_list_->getWidget());

    auto showButton = new QPushButton("Show Tree");
    layout->addWidget(showButton);
    connect(showButton, &QPushButton::clicked, [this]() {
        for (auto execution : execution_list_->getSelected())
        {
            showTraditionalView(execution);
        }
    });

    auto mergeButton = new QPushButton("Merge Trees");
    layout->addWidget(mergeButton);

    connect(mergeButton, &QPushButton::clicked, [this]() {
        const auto selected = execution_list_->getSelected();

        if (selected.size() == 2)
        {
            mergeTrees(selected[0], selected[1]);
        }
        else
        {
            print("select exactly two executions");
        }
    });

    auto saveButton = new QPushButton("Save Execution");
    layout->addWidget(saveButton);

    /// TODO: make it clear that only the first one is saved
    connect(saveButton, &QPushButton::clicked, [this]() {
        for (auto e : execution_list_->getSelected())
        {
            saveExecution(e);
            break;
        }
    });

    auto loadButton = new QPushButton("Load Execution");
    layout->addWidget(loadButton);

    connect(loadButton, &QPushButton::clicked, [this]() {
        const auto fileName = QFileDialog::getOpenFileName(this, "Open Execution").toStdString();

        if (fileName == "")
            return;

        auto ex = db_handler::load_execution(fileName.c_str());

        if (!ex)
        {
            print("could not load the execution");
        }
        else
        {
            addNewExecution(ex);
        }
    });

    server_.reset(new TcpServer([this](intptr_t socketDesc) {
        {
            /// Initiate a receiver thread
            auto receiver = new ReceiverThread(socketDesc, settings_);
            /// Delete the receiver one the thread is finished
            connect(receiver, &QThread::finished, receiver, &QObject::deleteLater);
            /// Handle the start message in this connector
            connect(receiver, &ReceiverThread::notifyStart, [this, receiver](const std::string &ex_name, int ex_id, bool restarts) {
                handleStart(receiver, ex_name, ex_id, restarts);
            });

            receiver->start();
        }
    }));

    listen_port_ = DEFAULT_PORT;

    // See if the default port is available
    auto res = server_->listen(QHostAddress::Any, listen_port_);
    if (!res)
    {
        // If not, try any port
        server_->listen(QHostAddress::Any, 0);
        listen_port_ = server_->serverPort();
    }

    std::cerr << "Ready to listen on: " << listen_port_ << std::endl;
}

static int getRandomExID()
{
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist(100);

    return dist(rng);
}

void Conductor::onExecutionDone(Execution *e) const
{

    print("on execution done");

    if (options_.save_search_path != "")
    {
        print("saving search to: {}", options_.save_search_path);
        saveSearch(e, options_.save_search_path.c_str());
        QApplication::quit();
    }
}

int Conductor::getNextExecId() const
{
    int eid = 0;
    while (executions_.find(eid) != executions_.end())
    {
        eid++;
    }
    return eid;
}

void Conductor::setMetaData(int exec_id, const std::string &group_name,
                            const std::string &exec_name,
                            std::shared_ptr<NameMap> nm)
{
    exec_meta_.insert({exec_id, {group_name, exec_name, nm}});

    debug("force") << "exec_id:" << exec_id << std::endl;
    debug("force") << "gr_name:" << group_name << std::endl;
    debug("force") << "ex_name:" << exec_name << std::endl;
}

int Conductor::getListenPort() const
{
    return static_cast<int>(listen_port_);
}

Conductor::~Conductor()
{

    debug("memory") << "~Conductor\n";
}

void Conductor::handleStart(ReceiverThread *receiver, const std::string &ex_name, int ex_id, bool restarts)
{

    auto res = executions_.find(ex_id);

    if (res == executions_.end() || ex_id == 0)
    {

        /// Note: metadata from MiniZinc IDE overrides that provided by the solver
        std::string ex_name_used = ex_name;

        const bool ide_used = (exec_meta_.find(ex_id) != exec_meta_.end());

        if (ide_used)
        {
            debug("force") << "already know metadata for this ex_id!\n";
            ex_name_used = exec_meta_[ex_id].ex_name;
        }

        /// needs a new execution
        auto ex = addNewExecution(ex_name_used, ex_id, restarts);

        /// construct a name map
        if (ide_used)
        {
            ex->setNameMap(exec_meta_[ex_id].name_map);
        }
        else if (options_.paths != "" && options_.mzn != "")
        {
            auto nm = std::make_shared<NameMap>();
            auto success = nm->initialize(options_.paths, options_.mzn);
            if (success)
            {
                ex->setNameMap(nm);
            }
        }

        /// The builder should only be created for a new execution
        auto builderThread = new QThread();
        auto builder = new TreeBuilder(*ex);

        builders_[ex_id] = builder;
        builder->moveToThread(builderThread);

        connect(builder, &TreeBuilder::buildingDone, [this, ex]() {
            onExecutionDone(ex);
        });

        /// is this the right time to delete the builder thread?
        connect(builderThread, &QThread::finished, builderThread, &QObject::deleteLater);

        builderThread->start();
    }

    /// obtain the builder aready assigned to this execution
    ///(either just now or by another connection)
    auto builder = builders_[ex_id];

    connect(receiver, &ReceiverThread::newNode,
            builder, &TreeBuilder::handleNode);

    connect(receiver, &ReceiverThread::doneReceiving,
            builder, &TreeBuilder::finishBuilding);
}

void Conductor::addNewExecution(std::shared_ptr<Execution> ex)
{

    auto ex_id = getRandomExID();

    debug("force") << "EXECUTION_ID: " << ex_id << std::endl;

    executions_[ex_id] = ex;
    execution_list_->addExecution(*ex);
}

Execution *Conductor::addNewExecution(const std::string &ex_name, int ex_id, bool restarts)
{

    auto ex = std::make_shared<Execution>(ex_name, restarts);

    if (ex_id == 0)
    {
        ex_id = getRandomExID();
    }

    debug("force") << "EXECUTION_ID: " << ex_id << std::endl;

    executions_[ex_id] = ex;
    execution_list_->addExecution(*ex);

    const bool auto_show = true;
    if (auto_show && options_.save_search_path == "")
    {
        showTraditionalView(ex.get());
    }

    return ex.get();
}

ExecutionWindow &Conductor::getExecutionWindow(Execution *e)
{
    auto maybe_view = execution_windows_.find(e);

    /// create new one if doesn't already exist
    if (maybe_view == execution_windows_.end())
    {

        execution_windows_[e] = utils::make_unique<ExecutionWindow>(*e);

        const auto ex_window = execution_windows_[e].get();

        connect(ex_window, &ExecutionWindow::needToSaveSearch, [this, e]() {
            saveSearch(e);
        });
    }

    return *execution_windows_.at(e);
}

void Conductor::mergeTrees(Execution *e1, Execution *e2)
{

    /// create new tree

    // QProgressDialog dialog;

    auto window = new analysis::MergeWindow();

    auto &new_tree = window->getTree();
    auto &res = window->mergeResult();

    /// Note: TreeMerger will delete itself when finished
    auto merger = new analysis::TreeMerger(*e1, *e2, new_tree, res);

    connect(merger, &analysis::TreeMerger::finished,
            window, &analysis::MergeWindow::finalize);
    merger->start();

    window->show();
}

void Conductor::saveSearch(Execution *e, const char *path) const
{

    const auto &nt = e->tree();

    const auto order = utils::pre_order(nt);

    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Truncate))
    {
        print("Error: could not open \"{}\" to save search, path");
        return;
    }

    QTextStream file_stream(&file);

    for (auto nid : order)
    {

        /// Making sure undefined/skipped nodes are not logged
        {
            const auto status = nt.getStatus(nid);
            if (status == tree::NodeStatus::SKIPPED || status == tree::NodeStatus::UNDETERMINED)
                continue;
        }

        // Note: not every child is logged (SKIPPED and UNDET are not)
        int kids_logged = 0;

        /// Note: this temporary stream is used so that children can be
        /// traversed first, counted, but logged after their parent
        std::stringstream children_stream;

        const auto kids = nt.childrenCount(nid);

        for (auto alt = 0; alt < kids; alt++)
        {

            const auto kid = nt.getChild(nid, alt);

            /// Making sure undefined/skipped children are not logged
            const auto status = nt.getStatus(kid);
            if (status == tree::NodeStatus::SKIPPED || status == tree::NodeStatus::UNDETERMINED)
                continue;

            ++kids_logged;
            /// TODO: use original names in labels
            const auto label = nt.getLabel(kid);

            children_stream << " " << kid << " " << label;
        }

        file_stream << nid << " " << kids_logged;

        /// Unexplored node on the left branch (search timed out)
        if ((kids == 0) && (nt.getStatus(nid) == tree::NodeStatus::BRANCH))
        {
            file_stream << " stop";
        }

        file_stream << children_stream.str().c_str() << '\n';
    }
}

void Conductor::saveSearch(Execution *e) const
{

    const auto file_path = QFileDialog::getSaveFileName(nullptr, "Save Search To a File").toStdString();

    saveSearch(e, file_path.c_str());
}

void Conductor::saveExecution(Execution *e)
{

    const auto file_path = QFileDialog::getOpenFileName(nullptr, "Open Execution").toStdString();

    db_handler::save_execution(e, file_path.c_str());

    qDebug() << "execution saved";
}

void Conductor::readSettings()
{

    QFile file("settings.json");

    if (!file.exists())
    {
        qDebug() << "settings.json not found";
        return;
    }

    file.open(QIODevice::ReadWrite | QIODevice::Text);

    auto data = file.readAll();

    auto json_doc = QJsonDocument::fromJson(data);

    if (json_doc.isEmpty())
    {
        qDebug() << "settings.json is empty";
        return;
    }

    if (json_doc.isObject())
    {

        auto json_obj = json_doc.object();

        settings_.receiver_delay = json_obj["receiver_delay"].toInt();
    }

    qDebug() << "settings read";
}

} // namespace cpprofiler

namespace cpprofiler
{

void Conductor::showTraditionalView(Execution *e)
{

    auto &window = getExecutionWindow(e);

    window.show();
}
} // namespace cpprofiler
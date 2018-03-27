#include "builder_thread.hh"
#include "cpp-integration/message.hpp"
#include "tree/structure.hh"
#include "tree/node_info.hh"
#include "tree/node_id.hh"

#include "utils/perf_helper.hh"

#include <iostream>
#include <QDebug>

namespace cpprofiler {

    static std::ostream& operator<<(std::ostream& os, const NodeUID& uid) {
        return os << "{" << uid.nid << ", " << uid.rid << ", " << uid.tid << "}";
    }

    static std::ostream& operator<<(std::ostream& os, const NodeStatus& status) {
        switch (status) {
            case SOLVED: os << "SOLVED"; break;
            case FAILED: os << "FAILED"; break;
            case BRANCH: os << "BRANCH"; break;
            case SKIPPED: os << "SKIPPED"; break;
        }
        return os;
    }

    /// works correctly for node messages only atm
    static std::ostream& operator<<(std::ostream& os, const Message& msg) {
        os << "nid: " << msg.nodeUID() << ", pid: " << msg.parentUID();
        os << ", alt: " << msg.alt() << ", kids: " << msg.kids();
        os << ", " << msg.status();
        // if (msg.has_label()) os << ", label: " << msg.label();
        // if (msg.has_nogood()) os << ", nogood: " << msg.nogood();
        // if (msg.has_info()) os << ", info: " << msg.info();
        return os;
    }

    BuilderThread::BuilderThread() {
    }


    void BuilderThread::run() {
        std::cerr << "Builder::run\n";

        exec();
    }

    void BuilderThread::startBuilding(Execution* e) {
        m_execution = e;
        perfHelper.begin("tree building");
        std::cerr << "Builder: execution ready\n";
    }

    void BuilderThread::finishBuilding() {
        perfHelper.end();
        std::cerr << "Builder: building done\n";
        this->quit();
    }

    void BuilderThread::handleNode(Message* node) {
        std::unique_ptr<Message> node_msg{node};
        // std::cerr << *node << std::endl;

        auto n_uid = node->nodeUID();
        auto p_uid = node->parentUID();

        auto& tree = m_execution->tree();

        tree::NodeID pid = tree::NodeID::NoNode;

        if (p_uid.nid != -1) {
            pid = m_execution->solver_data().getNodeId({p_uid.nid, p_uid.rid, p_uid.tid});
        }

        const auto nid = tree.addNode(pid, node->alt(), node->kids(),
            static_cast<tree::NodeStatus>(node->status()));

        m_execution->solver_data().setNodeId({n_uid.nid, n_uid.rid, n_uid.tid}, nid);

    }

    BuilderThread::~BuilderThread() {
        std::cerr << "~BuilderThread\n";
    }


}
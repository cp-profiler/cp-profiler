
#include "execution.hh"
#include "user_data.hh"
#include "tree/node_tree.hh"
#include "utils/debug.hh"

#include <iostream>

namespace cpprofiler {

    std::string Execution::name() {
        return m_name;
    }

    Execution::Execution(const std::string& name, bool restarts)
        : m_name{name}, m_tree{new tree::NodeTree()}, m_is_restarts(restarts)
    {



        /// need to create a dummy root node
        // TODO: make number of children dynamic

        if (restarts) {
            m_tree->createRoot(0, "root");
        }

        debug("memory") << "Execution()\n";
    }

    Execution::~Execution() {
        debug("memory") << "~Execution()\n";
    }

    const SolverData& Execution::solver_data() const {
        return m_solver_data;
    }

    SolverData& Execution::solver_data() {
        return m_solver_data;
    }

    const tree::NodeInfo& Execution::node_info() const {
        return m_tree->node_info();
    }

    tree::NodeInfo& Execution::node_info() {
        return m_tree->node_info();
    }

    tree::NodeTree& Execution::tree() {
        return *m_tree;
    }

    const tree::NodeTree& Execution::tree() const {
        return *m_tree;
    }

    bool Execution::doesRestarts() const {
        return m_is_restarts;
    }
}

namespace cpprofiler {

    void IdMap::addPair(SolverID sid, tree::NodeID nid) {
        QWriteLocker locker(&m_lock);
        m_uid2id[sid] = nid;
    }

    tree::NodeID IdMap::get(SolverID sid) const {
        QReadLocker locker(&m_lock);
        return m_uid2id.at(sid);
    }
}


namespace cpprofiler {

    const std::string SolverData::no_label = "";

    tree::NodeID SolverData::getNodeId(SolverID sid) const {
        return m_id_map.get(sid);
    }

    void SolverData::setNodeId(SolverID sid, tree::NodeID nid) {
        m_id_map.addPair(sid, nid);
    }

    void SolverData::setLabel(tree::NodeID nid, const std::string& label) {
        m_labels[nid] = label;
    }

    const std::string& SolverData::getLabel(tree::NodeID nid) const {
        if (m_labels.find(nid) != m_labels.end()) {
            return m_labels.at(nid);
        }
        return no_label;
    }

}
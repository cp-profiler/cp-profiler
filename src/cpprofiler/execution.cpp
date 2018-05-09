
#include "execution.hh"
#include "name_map.hh"
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
        if (restarts) {
            m_tree->createRoot(0, "root");
        }

        debug("memory") << "Execution()\n";
    }

    Execution::~Execution() {
        debug("memory") << "~Execution()\n";
    }

    void Execution::setNameMap(std::shared_ptr<const NameMap> nm) {
        name_map_ = nm;
        m_tree->setNameMap(nm);
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

    tree::NodeID SolverData::getNodeId(SolverID sid) const {
        return m_id_map.get(sid);
    }

    void SolverData::setNodeId(SolverID sid, tree::NodeID nid) {
        m_id_map.addPair(sid, nid);
    }


}
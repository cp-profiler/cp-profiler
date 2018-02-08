
#include "execution.hh"
#include "tree/node_tree.hh"

#include <iostream>

namespace cpprofiler {

    std::string Execution::name() {
        return m_name;
    }

    Execution::Execution(const std::string& name)
        : m_name{name}, m_tree{new tree::NodeTree()}
    {
        std::cerr << "Execution()\n";
    }

    Execution::~Execution() {
        std::cerr << "~Execution()\n";
    }

    const SolverData& Execution::solver_data() const {
        return m_solver_data;
    }

    SolverData& Execution::solver_data() {
        return m_solver_data;
    }

    const tree::Structure& Execution::tree_structure() const {
        return m_tree->tree_structure();
    }

    tree::Structure& Execution::tree_structure() {
        return m_tree->tree_structure();
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
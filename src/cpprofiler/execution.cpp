
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
        : m_name{name},
          m_tree{new tree::NodeTree()},
          solver_data_(utils::make_unique<SolverData>()),
          user_data_(utils::make_unique<UserData>()),
          m_is_restarts(restarts)
    {

        m_tree->setSolverData(solver_data_);

        print("Execution()");

        /// need to create a dummy root node
        if (restarts) {
            m_tree->createRoot(0, "root");
        }
    }

    void Execution::setNameMap(std::shared_ptr<const NameMap> nm) {
        name_map_ = nm;
        m_tree->setNameMap(nm);
    }

    const SolverData& Execution::solver_data() const {
        return *solver_data_;
    }

    SolverData& Execution::solver_data() {
        return *solver_data_;
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

        m_uid2id.insert({sid, nid});
        nid2uid_.insert({nid, sid});
    }

    tree::NodeID IdMap::get(SolverID sid) const {
        QReadLocker locker(&m_lock);
        return m_uid2id.at(sid);
    }
}
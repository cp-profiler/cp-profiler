#ifndef CPPROFILER_EXECUTION_HH
#define CPPROFILER_EXECUTION_HH

#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <QReadWriteLock>
#include "tree/node_tree.hh"
#include "tree/node.hh"

namespace cpprofiler {

    /// Mimics UID
    struct SolverID {
          // Node number
      int32_t nid;
      // Restart id
      int32_t rid;
      // Thread id
      int32_t tid;
    };

    static bool operator==(const SolverID& lhs, const SolverID& rhs) {
        return (lhs.nid == rhs.nid) && (lhs.tid == rhs.tid) && (lhs.rid == rhs.rid);
    }

}

namespace std
{
    template<> struct hash<cpprofiler::SolverID>
    {
        size_t operator()(cpprofiler::SolverID const& a) const
        {
            const size_t h1 ( std::hash<int>{}(a.nid) );
            size_t const h2 ( std::hash<int>{}(a.tid) );
            size_t const h3 ( std::hash<int>{}(a.rid) );
            return h1 ^ (h2 << 1) ^ (h3 << 1);
        }
    };
}


namespace cpprofiler {

class NameMap;

class IdMap {

    mutable QReadWriteLock m_lock;

    std::unordered_map<SolverID, tree::NodeID> m_uid2id;

    public:

    void addPair(SolverID, tree::NodeID);

    tree::NodeID get(SolverID) const;
};

class SolverData {

    IdMap m_id_map;

public:

    tree::NodeID getNodeId(SolverID sid) const;
    void setNodeId(SolverID sid, tree::NodeID nid);

};

class Execution {
    std::string m_name;

    std::unique_ptr<tree::NodeTree> m_tree;

    /// UID to NodeID map and labels (needs mutex protection?)
    SolverData m_solver_data;

    std::shared_ptr<const NameMap> name_map_;

    /// Whether the execution contains restarts
    bool m_is_restarts;

public:
    std::string name();

    Execution(const std::string& name, bool restarts = false);
    ~Execution();

    void setNameMap(std::shared_ptr<const NameMap> nm);

    SolverData& solver_data();
    const SolverData& solver_data() const;

    tree::NodeInfo& node_info();
    const tree::NodeInfo& node_info() const;

    tree::NodeTree& tree();
    const tree::NodeTree& tree() const;

    bool doesRestarts() const;
};

}



#endif


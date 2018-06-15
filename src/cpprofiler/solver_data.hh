#pragma once

#include <QReadWriteLock>
#include <unordered_map>

#include "core.hh"

#include "solver_id.hh"

namespace cpprofiler
{

class NameMap;

class IdMap
{

    mutable QReadWriteLock m_lock;

    std::unordered_map<SolverID, NodeID> uid2id_;

    std::unordered_map<NodeID, SolverID> nid2uid_;

  public:
    void addPair(SolverID, NodeID);

    NodeID get(SolverID) const;

    SolverID getUID(NodeID nid) const
    {
        auto it = nid2uid_.find(nid);

        if (it != nid2uid_.end())
        {
            return nid2uid_.at(nid);
        }
        else
        {
            return {-1, -1, -1};
        }
    }
};

class SolverData
{

    /// TODO:save/load id map to/from DB
    IdMap m_id_map;

    std::unordered_map<NodeID, Nogood> nogood_map_;

    /// Contributing constraints
    std::unordered_map<NodeID, std::vector<int>> contrib_cs_;

  public:
    NodeID getNodeId(SolverID sid) const
    {
        return m_id_map.get(sid);
    }

    void setNodeId(SolverID sid, NodeID nid)
    {
        m_id_map.addPair(sid, nid);
    }

    SolverID getSolverID(NodeID nid) const
    {
        return m_id_map.getUID(nid);
    }

    const std::vector<int> *getContribConstraints(NodeID nid) const
    {

        const auto it = contrib_cs_.find(nid);

        if (it == contrib_cs_.end())
            return nullptr;
        return &(it->second);
    }

    /// Associate nogood `ng` with node `nid`
    void setNogood(NodeID nid, const Nogood &ng)
    {
        nogood_map_.insert({nid, ng});
    }

    const Nogood &getNogood(NodeID nid) const
    {
        if (nogood_map_.find(nid) != nogood_map_.end())
        {
            return nogood_map_.at(nid);
        }
        else
        {
            return Nogood::empty;
        }
    }

    /// Process node info looking for reasons, contributing nogoods for failed nodes etc.
    void processInfo(NodeID nid, const std::string &info_str);

    /// Whether the data stores at least one no-good
    bool hasNogoods() const { return !nogood_map_.empty(); }
};

} // namespace cpprofiler
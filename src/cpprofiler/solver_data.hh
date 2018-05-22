#pragma once

#include <QReadWriteLock>
#include <unordered_map>

#include "core.hh"

namespace cpprofiler
{

/// Mimics UID
struct SolverID
{
    // Node number
    int32_t nid;
    // Restart id
    int32_t rid;
    // Thread id
    int32_t tid;

    std::string toString() const
    {
        return std::string("{") + std::to_string(nid) +
               ", " + std::to_string(rid) + ", " + std::to_string(tid) + "}";
    }
};

static bool operator==(const SolverID &lhs, const SolverID &rhs)
{
    return (lhs.nid == rhs.nid) && (lhs.tid == rhs.tid) && (lhs.rid == rhs.rid);
}

} // namespace cpprofiler

namespace std
{
template <>
struct hash<cpprofiler::SolverID>
{
    size_t operator()(cpprofiler::SolverID const &a) const
    {
        const size_t h1(std::hash<int>{}(a.nid));
        size_t const h2(std::hash<int>{}(a.tid));
        size_t const h3(std::hash<int>{}(a.rid));
        return h1 ^ (h2 << 1) ^ (h3 << 1);
    }
};
} // namespace std

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

    /// Whether the data stores at least one no-good
    bool hasNogoods() const
    {
        return (nogood_map_.size() > 0);
    }
};

} // namespace cpprofiler
#ifndef CPPROFILER_TREE_NODE_ID_HH
#define CPPROFILER_TREE_NODE_ID_HH

#include <cstdint>
#include <functional>

namespace cpprofiler
{
namespace tree
{

struct NodeID
{
    int id;

    static NodeID NoNode;

    operator int() const { return id; }
    explicit NodeID(int nid = -1) : id(nid) {}
};

} // namespace tree
} // namespace cpprofiler

namespace std
{
template <>
struct hash<cpprofiler::tree::NodeID>
{
    size_t operator()(const cpprofiler::tree::NodeID &nid) const
    {
        return std::hash<int>{}(static_cast<int>(nid));
    }
};
} // namespace std

namespace cpprofiler
{
namespace tree
{
bool operator==(const NodeID &lhs, const NodeID &rhs);
}
} // namespace cpprofiler

#endif
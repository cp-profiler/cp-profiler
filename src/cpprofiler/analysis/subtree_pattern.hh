#pragma once
#include "../core.hh"

namespace cpprofiler
{
namespace analysis
{
struct SubtreePattern
{
  private:
    int size_;

  public:
    std::vector<NodeID> m_nodes;
    int m_height;
    /// number of nodes in one (first) of the subtrees

    explicit SubtreePattern(int height) : m_height(height)
    {
    }

    SubtreePattern() = default;

    int setSize(int size)
    {
        size_ = size;
    }

    int size() const
    {
        return size_;
    }

    int count() const
    {
        return m_nodes.size();
    }

    int height() const
    {
        return m_height;
    }

    NodeID first() const
    {
        return m_nodes.at(0);
    }

    const std::vector<NodeID> &nodes() const
    {
        return m_nodes;
    }
};
} // namespace analysis
} // namespace cpprofiler
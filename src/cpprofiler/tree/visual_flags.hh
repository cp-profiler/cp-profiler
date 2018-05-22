#pragma once

#include "../core.hh"
#include <set>

namespace cpprofiler
{
namespace tree
{

class VisualFlags
{

    std::vector<bool> label_shown_;

    std::vector<bool> node_hidden_;

    std::vector<bool> shape_highlighted_;

    /// Somewhat redundant given m_shape_highlighted, but it is
    /// more efficient for unhighlighting previously highlighted subtrees
    std::set<NodeID> highlighted_shapes_;

    void ensure_id_exists(NodeID id);

  public:
    void setLabelShown(NodeID nid, bool val);
    bool isLabelShown(NodeID nid) const;

    void setHidden(NodeID nid, bool val);
    bool isHidden(NodeID nid) const;

    void setHighlighted(NodeID nid, bool val);
    bool isHighlighted(NodeID nid) const;

    void unhighlightAll();
};

} // namespace tree
} // namespace cpprofiler
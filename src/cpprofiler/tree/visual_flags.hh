#pragma once

#include "../core.hh"
#include <set>

namespace cpprofiler { namespace tree {

class VisualFlags {

    std::vector<bool> m_label_shown;

    std::vector<bool> m_node_hidden;

    std::vector<bool> m_shape_highlighted;

    /// This is somewhat redundant given m_shape_highlighted, but
    /// it is more suitable for unhighlighting previously highlighted
    std::set<NodeID> m_highlighted_shapes;

    void ensure_id_exists(NodeID id);

public:
    void set_label_shown(NodeID nid, bool val);
    bool get_label_shown(NodeID nid) const;

    void set_hidden(NodeID nid, bool val);
    bool get_hidden(NodeID nid) const;

    void set_highlighted(NodeID nid, bool val);
    bool get_highlighted(NodeID nid) const;

    void unhighlight_all();
};



}}
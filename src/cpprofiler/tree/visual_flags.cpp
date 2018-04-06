#include "visual_flags.hh"

namespace cpprofiler { namespace tree {

    void VisualFlags::ensure_id_exists(NodeID nid) {
        auto id = static_cast<int>(nid);
        if (m_label_shown.size() < id + 1) {
            m_label_shown.resize(id + 1);
            m_node_hidden.resize(id + 1);
            m_shape_highlighted.resize(id + 1);
        }
    }


    void VisualFlags::set_label_shown(NodeID nid, bool val) {
        ensure_id_exists(nid);
        m_label_shown[nid] = val;
    }

    bool VisualFlags::get_label_shown(NodeID nid) const {

        // return false; // TODO
        // ensure_id_exists(nid);

        if (m_label_shown.size() <= nid) return false;

        return m_label_shown.at(nid);
    }

    void VisualFlags::set_hidden(NodeID nid, bool val) {
        ensure_id_exists(nid);
        m_node_hidden[nid] = val;
    }

    bool VisualFlags::get_hidden(NodeID nid) const {
        if (m_node_hidden.size() <= nid) return false;
        return m_node_hidden.at(nid);
    }

    void VisualFlags::set_highlighted(NodeID nid, bool val) {
        ensure_id_exists(nid);

        if (val) {
            m_highlighted_shapes.insert(nid);
        } else {
            m_highlighted_shapes.erase(nid);
        }
        m_shape_highlighted[nid] = val;
    }

    bool VisualFlags::get_highlighted(NodeID nid) const {

        if (m_shape_highlighted.size() <= nid) {
            return false;
        }
        return m_shape_highlighted[nid];
    }

    void VisualFlags::unhighlight_all() {
        for (auto nid : m_highlighted_shapes) {
            m_shape_highlighted[nid] = false;
        }

        m_highlighted_shapes.clear();
    }

}}

#include "layout_cursor.hh"
#include "../node_id.hh"
#include <iostream>
#include <QDebug>

#include "../layout.hh"
#include "../node_tree.hh"
#include "../structure.hh"
#include "../shape.hh"

namespace cpprofiler { namespace tree {

    LayoutCursor::LayoutCursor(NodeID start, const NodeTree& tree, Layout& lo)
        : UnsafeNodeCursor(start, tree), m_layout(lo), m_tree(tree.tree_structure()) {}

    bool LayoutCursor::mayMoveDownwards() {
        return UnsafeNodeCursor::mayMoveDownwards() &&
               m_layout.isDirty_unsafe(m_cur_node);
    }

    static int distance_between(const Shape& s1, const Shape& s2) {
        auto depth_left = s1.depth();
        auto depth_right = s2.depth();
        auto common_depth = std::min(depth_left, depth_right);

        auto max = 0;
        for (auto i = 0; i < common_depth; ++i) {
            auto cur_dist = s1[i].r - s2[i].l;
            if (cur_dist > max) max = cur_dist; 
        }

        return max + Layout::min_dist_x;
    }

    static std::unique_ptr<Shape, ShapeDeleter> combine_shapes(const Shape& s1, const Shape& s2, std::vector<int>& offsets) {

        auto depth_left = s1.depth();
        auto depth_right = s2.depth();

        auto max_depth = std::max(depth_left, depth_right);
        auto common_depth = std::min(depth_left, depth_right);

        // auto combined = utils::make_unique<Shape, ShapeDeleter>(max_depth+1);
        auto combined = std::unique_ptr<Shape, ShapeDeleter>(new Shape{max_depth+1});

        auto distance = distance_between(s1, s2);
        auto half_dist = distance / 2;

        {
            auto bb_left = std::min(s1.boundingBox().left - half_dist, s2.boundingBox().left + half_dist);
            auto bb_right = std::max(s1.boundingBox().right - half_dist, s2.boundingBox().right + half_dist);
            combined->setBoundingBox(BoundingBox{bb_left, bb_right});
        }

        (*combined)[0] = {-half_dist, half_dist};

        offsets[0] = -half_dist; offsets[1] = half_dist;

        for (auto depth = 0; depth < common_depth; ++depth) {
            (*combined)[depth+1] = {s1[depth].l - half_dist, s2[depth].r + half_dist};
        }

        if (max_depth != common_depth) {
            const auto& longer_shape = depth_left > depth_right ? s1 : s2;
            const int offset = depth_left > depth_right ? -half_dist : half_dist;

            for (auto depth = common_depth; depth < max_depth; ++depth) {
                (*combined)[depth+1] = {longer_shape[depth].l + offset,
                                        longer_shape[depth].r + offset};
            }
        }

        return std::move(combined);
    }

    void LayoutCursor::computeForNode(NodeID nid) {

        auto nkids = m_tree.getNumberOfChildren_unsafe(nid);

        if (nkids == 0) {
                m_layout.setShape_unsafe(nid, std::unique_ptr<Shape, ShapeDeleter>(&Shape::leaf));
        }

        if (nkids == 2) {

            auto kid_l = m_tree.getChild_unsafe(nid, 0);
            auto kid_r = m_tree.getChild_unsafe(nid, 1);

            const auto& s1 = m_layout.getShape_unsafe(kid_l);
            const auto& s2 = m_layout.getShape_unsafe(kid_r);

            std::vector<int> offsets(2);
            auto combined = combine_shapes(s1, s2, offsets);

            m_layout.setShape_unsafe(nid, std::move(combined));

            m_layout.setChildOffset_unsafe(kid_l, offsets[0]);
            m_layout.setChildOffset_unsafe(kid_r, offsets[1]);

            m_layout.setLayoutDone_unsafe(nid, true);

        }

    }

    void LayoutCursor::processCurrentNode() {
        // m_layout_order.push_back(m_cur_node);

        auto dirty = m_layout.isDirty_unsafe(m_cur_node);

        if (dirty) {
            // std::cerr << m_cur_node << " ";
            computeForNode(m_cur_node);
            m_layout.setDirty_unsafe(m_cur_node, false);
        }
    }

    void LayoutCursor::finalize() {
        // std::cerr << "\n";
    }


}}

#include "layout_cursor.hh"
#include "../../config.hh"
#include "../node_id.hh"
#include <iostream>
#include <QDebug>
#include <QPainter>

#include "../layout.hh"
#include "../node_tree.hh"
#include "../structure.hh"
#include "../shape.hh"
#include "../../config.hh"

/// needed for VisualFlags
#include "../traditional_view.hh"

#include <numeric>

namespace cpprofiler { namespace tree {

    LayoutCursor::LayoutCursor(NodeID start, const NodeTree& tree, const VisualFlags& nf, Layout& lo)
        : NodeCursor(start, tree), m_layout(lo), m_nt(tree), m_tree(tree.tree_structure()), m_vis_flags(nf) {}

    bool LayoutCursor::mayMoveDownwards() {
        return NodeCursor::mayMoveDownwards() &&
               !m_vis_flags.get_hidden(m_cur_node) &&
               m_layout.isDirty_unsafe(m_cur_node);
    }

    void LayoutCursor::setLabelPainter(QPainter* painter) {
        m_painter = painter;
    }

    /// Compute the distance between s1 and s2 (that is, how far apart should the
    /// corresponding root nodes be along X axis to avoid overlap, additionally
    /// allowing for some margin -- layout::min_dist_x)
    static int distance_between(const Shape& s1, const Shape& s2) {
        const auto common_depth = std::min(s1.depth(), s2.depth());

        auto max = 0;
        for (auto i = 0; i < common_depth; ++i) {
            auto cur_dist = s1[i].r - s2[i].l;
            if (cur_dist > max) max = cur_dist; 
        }

        return max + layout::min_dist_x;
    }

    using ShapeUniqPtr = std::unique_ptr<Shape, ShapeDeleter>;

    /// Combine shapes s1 and s2 to form a shape of a shape for the parent node;
    /// offsets will contain the resulting relative distance from the parent node (along x);
    static ShapeUniqPtr combine_shapes(const Shape& s1, const Shape& s2, std::vector<int>& offsets) {

        const auto depth_left = s1.depth();
        const auto depth_right = s2.depth();

        const auto max_depth = std::max(depth_left, depth_right);
        const auto common_depth = std::min(depth_left, depth_right);

        const auto distance = distance_between(s1, s2);
        const auto half_dist = distance / 2;

        auto combined = ShapeUniqPtr(new Shape{max_depth+1});

        {
            const auto bb_left = std::min(s1.boundingBox().left - half_dist,
                                          s2.boundingBox().left + half_dist);
            const auto bb_right = std::max(s1.boundingBox().right - half_dist,
                                           s2.boundingBox().right + half_dist);
            combined->setBoundingBox(BoundingBox{bb_left, bb_right});
        }

        offsets[0] = -half_dist; offsets[1] = half_dist;

        /// Calculate extents for levels shared by both shapes
        for (auto depth = 0; depth < common_depth; ++depth) {
            (*combined)[depth+1] = {s1[depth].l - half_dist, s2[depth].r + half_dist};
        }

        /// Calculate extents for levels where only one subtree has nodes
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

    /// Merge two "sibling" shapes, without adding a new depth level;
    /// `dist` is the disntance between s2 and the left extent
    static Shape merge_left(const Shape& s1, const Shape& s2, int& dist) {

        const auto depth_left = s1.depth();
        const auto depth_right = s2.depth();

        const auto common_depth = std::min(depth_left, depth_right);
        const auto max_depth = std::max(depth_left, depth_right);

        const auto distance = distance_between(s1, s2);

        dist = distance - s1.boundingBox().left;

        Shape result{max_depth};

        /// compute bounding box
        {
            auto bb_left = std::min(s1.boundingBox().left,
                                    s2.boundingBox().left + distance);
            auto bb_right = std::max(s1.boundingBox().right,
                                    s2.boundingBox().right + distance);
            result.setBoundingBox(BoundingBox{bb_left, bb_right});
        }

        /// Calculate extents for levels shared by both shapes
        for (auto depth = 0; depth < common_depth; ++depth) {
            result[depth] = {s1[depth].l, s2[depth].r + distance};
        }

        /// Calculate extents for levels where only one subtree has nodes
        if (max_depth != common_depth) {
            const auto& longer_shape = depth_left > depth_right ? s1 : s2;
            const int offset = depth_left > depth_right ? 0 : distance;

            for (auto depth = common_depth; depth < max_depth; ++depth) {
                result[depth] = {longer_shape[depth].l + offset,
                                        longer_shape[depth].r + offset};
            }
        }

        return result;
    }

    static Extent calculateForSingleNode(NodeID nid, const NodeTree& nt, const VisualFlags& nf) {

        Extent result{-traditional::HALF_MAX_NODE_W, traditional::HALF_MAX_NODE_W};
        /// see if the node dispays labels and needs its (top) extents extended
        if (nf.get_label_shown(nid)) {

            /// TODO: use font metrics?
            const auto& label = nt.getLabel(nid);
            auto label_width = label.size() * 9;

            /// Note: this assumes that the default painter used for drawing text
            // QPainter painter;
            // auto fm = painter.fontMetrics();
            // auto label_width = fm.width(label.c_str());

            /// Note that labels are shown on the left for all alt
            /// except the last one (right-most)
            bool draw_left = nt.isRightMostChild(nid) ? false : true;

            if (draw_left) {
                result.l -= label_width;
            } else {
                result.r += label_width;
            }
        }
        return result;
    }

    static inline void computeForNodeBinary(NodeID nid, Layout& layout, const NodeTree& nt, const VisualFlags& nf) {

        auto kid_l = nt.getChild(nid, 0);
        auto kid_r = nt.getChild(nid, 1);

        const auto& s1 = layout.getShape_unsafe(kid_l);
        const auto& s2 = layout.getShape_unsafe(kid_r);

        std::vector<int> offsets(2);
        auto combined = combine_shapes(s1, s2, offsets);
        (*combined)[0] = calculateForSingleNode(nid, nt, nf);

        /// Extents for root node changed -> check if bounding box is correct
        const auto& bb = combined->boundingBox();

        if (bb.left > (*combined)[0].l || bb.right < (*combined)[0].r) {
            combined->setBoundingBox({
                std::min(bb.left, (*combined)[0].l), std::max(bb.right, (*combined)[0].r)
            });
        }

        layout.setShape_unsafe(nid, std::move(combined));

        layout.setChildOffset_unsafe(kid_l, offsets[0]);
        layout.setChildOffset_unsafe(kid_r, offsets[1]);
    }

    static inline void computeForNodeNary(NodeID nid, int nkids, Layout& layout, const Structure& tree) {

        /// calculate all distances
        std::vector<int> distances(nkids - 1);

        for (auto i = 0; i < nkids - 1; ++i) {
            auto kid_l = tree.getChild(nid, i);
            auto kid_r = tree.getChild(nid, i+1);
            const auto& s1 = layout.getShape_unsafe(kid_l);
            const auto& s2 = layout.getShape_unsafe(kid_r);
            distances[i] = distance_between(s1, s2);
        }

        /// calculate total width (the sum of all distances + )
        int total_width = 0;
        const auto leftmost_kid = tree.getChild(nid, 0);
        const auto& leftmost_shape = layout.getShape_unsafe(leftmost_kid);
        {
            const auto rightmost_kid = tree.getChild(nid, nkids - 1);
            const auto& rightmost_shape = layout.getShape_unsafe(rightmost_kid);

            total_width -= leftmost_shape.boundingBox().left;
            for (auto distance : distances) {
                total_width += distance;
            }
            total_width += rightmost_shape.boundingBox().right;
        }

        const auto half_w = total_width / 2;

        /// calculate max depth
        int max_depth = 0;
        {
            for (auto i = 0; i < nkids; ++i) {
                auto kid_l = tree.getChild(nid, i);
                const auto& s1 = layout.getShape_unsafe(kid_l);
                max_depth = std::max(max_depth, s1.depth());
            }
        }

        const auto new_depth = max_depth + 1;

        auto combined = ShapeUniqPtr(new Shape{new_depth});

        combined->setBoundingBox({-half_w, half_w});

        std::vector<int> x_offsets(nkids);

        /// calculate offsets
        auto cur_x = -half_w - leftmost_shape.boundingBox().left;
        for (auto i = 0; i < nkids; ++i) {
            const auto kid = tree.getChild(nid, i);
            layout.setChildOffset_unsafe(kid, cur_x);
            x_offsets[i] = cur_x;
            cur_x += distances[i];
        }

        /// calculate extents
        (*combined)[0] = {-traditional::HALF_MAX_NODE_W, traditional::HALF_MAX_NODE_W};
        for (auto depth = 1; depth < new_depth; ++depth) {

            auto leftmost_x = 0;
            auto rightmost_x = 0;
            for (auto kid = 0; kid < nkids; ++kid) {
                const auto kid_id = tree.getChild(nid, kid);
                const auto& shape = layout.getShape_unsafe(kid_id);
                if (shape.depth() > depth - 1) {
                    leftmost_x = std::min(leftmost_x, shape[depth-1].l + x_offsets[kid]);
                    rightmost_x = std::max(leftmost_x, shape[depth-1].r + x_offsets[kid]);
                }
            }

            (*combined)[depth].l = leftmost_x;
            (*combined)[depth].r = rightmost_x;
        }


        layout.setShape_unsafe(nid, std::move(combined));


    }

    /// Computes layout for nid (shape, bounding box, offsets for its children)
    void LayoutCursor::computeForNode(NodeID nid) {

        /// Check if the node is hidden:
        if (m_vis_flags.get_hidden(nid)) {
            m_layout.setShape_unsafe(nid, ShapeUniqPtr(&Shape::hidden));
        } else {
            auto nkids = m_tree.childrenCount(nid);

            if (nkids == 0) {

                if (!m_vis_flags.get_label_shown(nid)) {
                    m_layout.setShape_unsafe(nid, ShapeUniqPtr(&Shape::leaf));
                } else {
                    /// TODO
                    auto shape = ShapeUniqPtr{new Shape{1}};
                    (*shape)[0] = calculateForSingleNode(nid, m_nt, m_vis_flags);

                    shape->setBoundingBox({(*shape)[0].l, (*shape)[0].r});
                    m_layout.setShape_unsafe(nid, std::move(shape));
                }
            }

            if (nkids == 1) {
                
                const auto kid = m_tree.getChild(nid, 0);
                const auto& kid_s = m_layout.getShape_unsafe(kid);

                auto shape = ShapeUniqPtr(new Shape(kid_s.depth() + 1));

                (*shape)[0] = {0, 0};

                for (auto depth = 0; depth < kid_s.depth(); depth++) {
                    (*shape)[depth+1] = kid_s[depth];
                }

                shape->setBoundingBox(kid_s.boundingBox());

                m_layout.setShape_unsafe(nid, std::move(shape));
            }

            if (nkids == 2) {
                computeForNodeBinary(nid, m_layout, m_nt, m_vis_flags);
            }

            if (nkids > 2) {
                computeForNodeNary(nid, nkids, m_layout, m_tree);
            }
        }

        m_layout.setLayoutDone_unsafe(nid, true);
    }

    void LayoutCursor::processCurrentNode() {
        // m_layout_order.push_back(m_cur_node);

        auto dirty = m_layout.isDirty_unsafe(m_cur_node);

        if (dirty) {
            computeForNode(m_cur_node);
            m_layout.setDirty_unsafe(m_cur_node, false);
        }
    }

    void LayoutCursor::finalize() {
        // std::cerr << "\n";
    }


}}
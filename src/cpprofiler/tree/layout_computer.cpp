#include "layout_computer.hh"
#include "layout.hh"
#include "structure.hh"
#include "shape.hh"
#include "../utils/std_ext.hh"

#include <QMutex>
#include <QDebug>
#include <QMutexLocker>
#include <iostream>

namespace cpprofiler { namespace tree {

LayoutComputer::LayoutComputer(const Structure& tree, Layout& layout, QMutex& layout_mutex)
: m_tree(tree), m_layout(layout), m_layout_mutex(layout_mutex)
{

}

std::unique_ptr<Shape> combine_shapes(const Shape& s1, const Shape& s2) {

    auto depth_left = s1.depth();
    auto depth_right = s2.depth();

    auto max_depth = std::max(depth_left, depth_right);
    auto common_depth = std::min(depth_left, depth_right);

    auto combined = utils::make_unique<Shape>(max_depth+1);

    (*combined)[0] = {-Layout::min_dist_x, Layout::min_dist_x};

    for (auto depth = 0; depth < common_depth; ++depth) {
        (*combined)[depth+1] = {s1[depth].l - Layout::min_dist_x, s2[depth].r + Layout::min_dist_x};
    }

    if (max_depth != common_depth) {
        const auto& longer_shape = depth_left > depth_right ? s1 : s2;
        const int offset = depth_left > depth_right ? -Layout::min_dist_x : Layout::min_dist_x;

        for (auto depth = common_depth; depth < max_depth; ++depth) {
            (*combined)[depth+1] = {longer_shape[depth].l + offset,
                                    longer_shape[depth].r + offset};
        }
    }

    return std::move(combined);
}

void LayoutComputer::compute() {
    QMutexLocker locker(&m_layout_mutex);

    auto order = helper::postOrder(m_tree);

    for (auto nid : order) {

        auto nkids = m_tree.getNumberOfChildren(nid);

        if (nkids == 0) {
            m_layout.setShape_unprotected(nid, &Shape::leaf);
            // continue;
        }

        if (nkids == 2) {

            auto kid_l = m_tree.getChild(nid, 0);
            auto kid_r = m_tree.getChild(nid, 1);

            const auto& s1 = m_layout.getShape_unprotected(kid_l);
            const auto& s2 = m_layout.getShape_unprotected(kid_r);

            auto combined = combine_shapes(s1, s2);

            m_layout.setShape_unprotected(nid, std::move(combined));

            m_layout.setChildOffset_unprotected(kid_l, -Layout::min_dist_x);
            m_layout.setChildOffset_unprotected(kid_r, Layout::min_dist_x);

        }
        // check if has children

        // get children

        /// get their extents

        auto& shape = m_layout.getShape_unprotected(nid);

        std::cerr << shape << "\n";

        // shape


    }

    
}


}}
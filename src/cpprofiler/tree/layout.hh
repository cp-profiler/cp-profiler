#ifndef CPPROFILER_TREE_LAYOUT_HH
#define CPPROFILER_TREE_LAYOUT_HH

#include <vector>
#include <memory>
#include <unordered_map>
#include <QMutex>

#include "node_id.hh"
#include "layout_computer.hh"

namespace cpprofiler { namespace tree {

class Shape;
class LayoutComputer;
class Structure;

class Layout {

    friend class LayoutComputer;

    mutable QMutex m_layout_mutex;

    std::unordered_map<NodeID, Shape*> m_shape_map;

    std::unordered_map<NodeID, std::unique_ptr<Shape>> m_shapes;

    /// Relative offset from the parent node along the x axis
    std::unordered_map<NodeID, double> m_child_offsets;

    void setChildOffset_unprotected(NodeID nid, double offset);

    const Shape& getShape_unprotected(NodeID nid) const;

    /// just remember the pointer, it is managed elsewhere
    void setShape_unprotected(NodeID nid, Shape* shape);

    /// take ownership over shape
    void setShape_unprotected(NodeID nid, std::unique_ptr<Shape> shape);

public:

    static constexpr double dist_y = 38.0;
    static constexpr int min_dist_x = 16;

    std::unique_ptr<LayoutComputer> createComputer(const Structure& str);

    double getOffset(NodeID nid) const;

    Layout();
    ~Layout();

};

}}


#endif
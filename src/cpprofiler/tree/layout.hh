#ifndef CPPROFILER_TREE_LAYOUT_HH
#define CPPROFILER_TREE_LAYOUT_HH

#include <vector>
#include <memory>
#include <unordered_map>
#include <QMutex>

#include "node_id.hh"

namespace cpprofiler { namespace tree {

class Shape;
class LayoutComputer;
class Structure;
class BoundingBox;

class Layout {

    friend class LayoutComputer;

    mutable QMutex m_layout_mutex;

    std::unordered_map<NodeID, Shape*> m_shape_map;

    std::unordered_map<NodeID, std::unique_ptr<Shape>> m_shapes;

    /// Relative offset from the parent node along the x axis
    std::unordered_map<NodeID, double> m_child_offsets;

    std::unordered_map<NodeID, bool> m_layout_done;

    void setChildOffset_unprotected(NodeID nid, double offset);

    void setLayoutDone_unprotected(NodeID nid, bool);

    bool getLayoutDone_unprotected(NodeID nid) const;

    const Shape& getShape_unprotected(NodeID nid) const;

    /// just remember the pointer, it is managed elsewhere
    void setShape_unprotected(NodeID nid, Shape* shape);

    QMutex& getMutex();

    /// take ownership over shape
    void setShape_unprotected(NodeID nid, std::unique_ptr<Shape> shape);

public:

    static constexpr int dist_y = 38;
    static constexpr int min_dist_x = 16;

    double getOffset(NodeID nid) const;

    int getDepth(NodeID nid) const;

    bool getLayoutDone(NodeID nid) const;

    const BoundingBox& getBoundingBox(NodeID nid) const;

    Layout();
    ~Layout();

};

}}


#endif
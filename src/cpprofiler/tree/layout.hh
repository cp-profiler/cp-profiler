#ifndef CPPROFILER_TREE_LAYOUT_HH
#define CPPROFILER_TREE_LAYOUT_HH

#include <vector>
#include <memory>
#include <unordered_map>
#include <QMutex>
#include <QObject>

#include "node_id.hh"

namespace cpprofiler { namespace tree {

class Shape;
class LayoutComputer;
class Structure;
class BoundingBox;
class ShapeDeleter;

class Layout : public QObject {
Q_OBJECT

    friend class LayoutComputer;

    mutable QMutex m_layout_mutex;

    std::vector<std::unique_ptr<Shape, ShapeDeleter>> m_shapes;

    /// Relative offset from the parent node along the x axis
    std::vector<double> m_child_offsets;

    std::vector<bool> m_layout_done;

    void setChildOffset_unsafe(NodeID nid, double offset);

    void setLayoutDone_unsafe(NodeID nid, bool);

    bool getLayoutDone_unsafe(NodeID nid) const;

    const Shape& getShape_unsafe(NodeID nid) const;

    /// just remember the pointer, it is managed elsewhere
    // void setShape_unsafe(NodeID nid, Shape* shape);

    QMutex& getMutex();

    /// take ownership over shape
    void setShape_unsafe(NodeID nid, std::unique_ptr<Shape, ShapeDeleter> shape);

public:

    static constexpr int dist_y = 38;
    static constexpr int min_dist_x = 16;

    double getOffset(NodeID nid) const;

    int getDepth(NodeID nid) const;

    bool getLayoutDone(NodeID nid) const;

    const BoundingBox& getBoundingBox(NodeID nid) const;

    Layout();
    ~Layout();

public slots:

    void growDataStructures(int n_nodes);

};

}}


#endif
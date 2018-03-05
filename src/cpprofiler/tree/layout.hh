#ifndef CPPROFILER_TREE_LAYOUT_HH
#define CPPROFILER_TREE_LAYOUT_HH

#include <vector>
#include <memory>
#include <unordered_map>
#include <QMutex>
#include <QObject>

#include "../core.hh"

namespace cpprofiler { namespace tree {

class Shape;
class LayoutComputer;
class Structure;
class BoundingBox;
class ShapeDeleter;


/// IMPORTANT: Methods marked as `unsafe` require the caller to hold m_layout_mutex
class Layout : public QObject {
Q_OBJECT

    mutable utils::Mutex m_layout_mutex;

    std::vector<std::unique_ptr<Shape, ShapeDeleter>> m_shapes;

    /// Relative offset from the parent node along the x axis
    std::vector<double> m_child_offsets;

    std::vector<bool> m_layout_done;
    std::vector<bool> m_dirty;

public:

    static constexpr int dist_y = 38;
    static constexpr int min_dist_x = 16;

    utils::Mutex& getMutex() const;

    void setChildOffset_unsafe(NodeID nid, double offset);

    void setLayoutDone_unsafe(NodeID nid, bool);

    const Shape& getShape_unsafe(NodeID nid) const;

    void setShape_unsafe(NodeID nid, std::unique_ptr<Shape, ShapeDeleter> shape);

    double getOffset(NodeID nid) const;
    double getOffset_unsafe(NodeID nid) const;

    int getDepth(NodeID nid) const;
    int getDepth_unsafe(NodeID nid) const;

    bool getLayoutDone(NodeID nid) const;
    bool getLayoutDone_unsafe(NodeID nid) const;

    bool isDirty_unsafe(NodeID nid) const;

    void setDirty_unsafe(NodeID nid, bool val);

    const BoundingBox& getBoundingBox(NodeID nid) const;
    const BoundingBox& getBoundingBox_unsafe(NodeID nid) const;

    Layout();
    ~Layout();

public slots:

    void growDataStructures(int n_nodes);

};

}}


#endif
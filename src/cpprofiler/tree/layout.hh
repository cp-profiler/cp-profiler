#ifndef CPPROFILER_TREE_LAYOUT_HH
#define CPPROFILER_TREE_LAYOUT_HH

#include <vector>
#include <memory>
#include <unordered_map>
#include <QMutex>
#include <QObject>

#include "../core.hh"
#include "shape.hh"

namespace cpprofiler
{
namespace tree
{

class Shape;
class LayoutComputer;
class Structure;
class BoundingBox;
class ShapeDeleter;

using ShapeUniqPtr = std::unique_ptr<Shape, ShapeDeleter>;

class Layout : public QObject
{
    Q_OBJECT

    mutable utils::Mutex m_layout_mutex;

    /// TODO: make sure this is always protected by a mutex
    std::vector<ShapeUniqPtr> m_shapes;

    /// Relative offset from the parent node along the x axis
    std::vector<double> m_child_offsets;

    /// Whether layout for the node and its children is done (indexed by NodeID)
    std::vector<bool> m_layout_done;

    /// Whether a node's shape need to be recomputed (indexed by NodeID)
    std::vector<bool> m_dirty;

  public:
    utils::Mutex &getMutex() const;

    void setChildOffset(NodeID nid, double offset) { m_child_offsets[nid] = offset; }

    void setLayoutDone(NodeID nid, bool val) { m_layout_done[nid] = val; }

    /// Note: a node might not have a shape (nullptr)
    /// if it was hidden before layout was run
    const Shape *getShape(NodeID nid) const { return m_shapes[nid].get(); }

    void setShape(NodeID nid, ShapeUniqPtr shape);

    double getOffset(NodeID nid) const { return m_child_offsets[nid]; }

    /// Get the height of the shape of node `nid`
    int getHeight(NodeID nid) const;

    /// Whether layout is done for node `nid`
    bool getLayoutDone(NodeID nid) const { return m_layout_done[nid]; }

    /// Whether layout is (at least partially) ready to be used for drawing
    bool ready() const;

    /// Whether node `nid` is dirty
    bool isDirty(NodeID nid) const { return m_dirty[nid]; }

    /// Set node `nid` as (dirty) / (not dirty) based on `val`
    void setDirty(NodeID nid, bool val) { m_dirty[nid] = val; }

    /// Get bounding box of node `nid`
    const BoundingBox &getBoundingBox(NodeID nid) const { return getShape(nid)->boundingBox(); }

    Layout();
    ~Layout();

  public slots:

    void growDataStructures(int n_nodes);
};

} // namespace tree
} // namespace cpprofiler

#endif
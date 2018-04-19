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

using ShapeUniqPtr = std::unique_ptr<Shape, ShapeDeleter>;

class Layout : public QObject {
Q_OBJECT

    mutable utils::Mutex m_layout_mutex;

    /// TODO: make sure this is always protected by a mutex
    std::vector<std::unique_ptr<Shape, ShapeDeleter>> m_shapes;

    /// Relative offset from the parent node along the x axis
    std::vector<double> m_child_offsets;

    /// Indicates whether layout for the node and its children is done
    std::vector<bool> m_layout_done;

    std::vector<bool> m_dirty;

public:

    utils::Mutex& getMutex() const;

    void setChildOffset(NodeID nid, double offset);

    void setLayoutDone(NodeID nid, bool);

    const Shape& getShape(NodeID nid) const;

    void setShape(NodeID nid, std::unique_ptr<Shape, ShapeDeleter> shape);

    double getOffset(NodeID nid) const;

    int getDepth(NodeID nid) const;

    bool getLayoutDone(NodeID nid) const;

    bool isDirty(NodeID nid) const;

    void setDirty(NodeID nid, bool val);

    const BoundingBox& getBoundingBox(NodeID nid) const;

    Layout();
    ~Layout();

public slots:

    void growDataStructures(int n_nodes);

};

}}


#endif
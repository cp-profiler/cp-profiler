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

    QMutex m_layout_mutex;

    std::unordered_map<NodeID, Shape*> m_shape_map;

    std::vector<std::unique_ptr<Shape>> m_shapes;

    const Shape& getShape_unprotected(NodeID nid) const;

    /// just remember the pointer, it is managed elsewhere
    void setShape_unprotected(NodeID nid, Shape* shape);

    /// take ownership over shape
    void setShape_unprotected(NodeID nid, std::unique_ptr<Shape> shape);

public:


    std::unique_ptr<LayoutComputer> createComputer(const Structure& str);

    Layout();
    ~Layout();

};

}}


#endif
#ifndef CPPROFILER_TREE_LAYOUT_HH
#define CPPROFILER_TREE_LAYOUT_HH

#include <vector>
#include <memory>
#include <memory>
#include <QMutex>

#include "node_id.hh"

namespace cpprofiler { namespace tree {

class Shape;

class Layout {

    QMutex m_layout_mutex;

    std::vector<std::unique_ptr<Shape>> m_shapes;

public:

    /// NOTE(maxim) cannot return Shape& without protecting it with a mutex
    void getShape(NodeID nid);

    void setShape(NodeID nid, const Shape& shape);

    ~Layout();

};

}}


#endif
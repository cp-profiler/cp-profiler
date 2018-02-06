#ifndef CPPROFILER_TREE_LAYOUT_HH
#define CPPROFILER_TREE_LAYOUT_HH

#include <vector>
#include <memory>
#include "shape.hh"

namespace cpprofiler { namespace tree {

class Layout {

    std::vector<std::unique_ptr<Shape>> shapes;

};

}}


#endif
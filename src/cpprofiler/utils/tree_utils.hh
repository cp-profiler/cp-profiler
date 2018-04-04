#pragma once

#include "../core.hh"

namespace cpprofiler { namespace tree {
    class NodeTree;
}}

namespace cpprofiler { namespace utils {

int count_descendants(const tree::NodeTree& nt, NodeID n);


}}
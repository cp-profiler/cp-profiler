#pragma once

#include "../core.hh"
#include <functional>

using NodeAction = std::function<void(NodeID)>;

namespace cpprofiler { namespace tree {
    class NodeTree;
}}

namespace cpprofiler { namespace utils {

int count_descendants(const tree::NodeTree& nt, NodeID n);

void apply_for_all_descendants(const tree::NodeTree& nt, NodeID root, const NodeAction& action);


}}
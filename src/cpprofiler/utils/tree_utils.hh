#pragma once

#include "../core.hh"
#include <functional>

using NodeAction = std::function<void(NodeID)>;

namespace cpprofiler { namespace tree {
    class NodeTree;
}}

namespace cpprofiler { namespace utils {

/// count all descendants of `n`
int count_descendants(const tree::NodeTree& nt, NodeID n);

/// apply `action` to `root` and to all of its descendants
void apply_below(const tree::NodeTree& nt, NodeID root, const NodeAction& action);


}}
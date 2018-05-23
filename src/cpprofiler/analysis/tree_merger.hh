#pragma once

#include <QThread>
#include <memory>

#include "merging/merge_result.hh"

namespace cpprofiler
{

namespace tree
{
class NodeTree;
}

class Execution;
} // namespace cpprofiler

namespace cpprofiler
{
namespace analysis
{

class TreeMerger : public QThread
{

  const Execution &ex_l;
  const Execution &ex_r;

  const tree::NodeTree &tree_l;
  const tree::NodeTree &tree_r;

  std::shared_ptr<tree::NodeTree> res_tree;
  std::shared_ptr<MergeResult> merge_result;

protected:
  void run() override;

public:
  TreeMerger(const Execution &ex_l,
             const Execution &ex_r,
             std::shared_ptr<tree::NodeTree> tree,
             std::shared_ptr<analysis::MergeResult> res);
  ~TreeMerger();
};

} // namespace analysis
} // namespace cpprofiler
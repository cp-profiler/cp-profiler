#ifndef CPPROFILER_TREE_LAYOUT_COMPUTER_HH
#define CPPROFILER_TREE_LAYOUT_COMPUTER_HH

class QMutex;

#include "node_id.hh"

namespace cpprofiler
{
namespace tree
{

class Layout;
class NodeTree;
class VisualFlags;

class LayoutComputer
{

    const NodeTree &m_tree;
    const VisualFlags &m_vis_flags;
    Layout &m_layout;

    bool m_needs_update = true;

  public:
    LayoutComputer(const NodeTree &tree, Layout &layout, const VisualFlags &nf);

    /// compute the layout and return where any work was required
    bool compute();

    void dirtyUp(NodeID nid);

    bool isDirty(NodeID nid);

    void setDirty(NodeID nid);
};

} // namespace tree
} // namespace cpprofiler

#endif
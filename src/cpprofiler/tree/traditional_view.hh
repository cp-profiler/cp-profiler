#ifndef CPPROFILER_TREE_TRADITIONAL_VIEW_HH
#define CPPROFILER_TREE_TRADITIONAL_VIEW_HH

#include <QWidget>

#include <memory>
#include <set>
#include "node_id.hh"
#include "visual_flags.hh"

namespace cpprofiler
{
class UserData;
class SolverData;
} // namespace cpprofiler

namespace cpprofiler
{
namespace tree
{

class Layout;
class LayoutComputer;
class NodeTree;
class NodeID;
class Structure;

class TreeScrollArea;

class TraditionalView : public QObject
{
    Q_OBJECT

    /// Tree structure
    const NodeTree &tree_;

    /// User data (bookmarks etc.)
    UserData &user_data_;

    /// Nogoods, solver ids
    SolverData &solver_data_;

    /// Visual flags (hidden/highlighted etc) per node
    std::unique_ptr<VisualFlags> vis_flags_;

    /// Layout: shapes of every node
    std::unique_ptr<Layout> layout_;

    /// Responsible for keeping the layout up to date
    std::unique_ptr<LayoutComputer> layout_computer_;

    /// The area the tree is actually drawn onto
    std::unique_ptr<TreeScrollArea> scroll_area_;

    /// Only update layout if it is stale
    bool layout_stale_ = true;

    /// Returns currently selected node; can be NodeID::NoNode
    NodeID node() const;

    /// Sets nid as the currently selected node
    void setNode(NodeID nid);

  public:
    TraditionalView(const NodeTree &tree, UserData &ud, SolverData &sd);
    ~TraditionalView();

    /// Return the scroll area
    QWidget *widget();

    /// Show the label for node `nid` causing layout update
    void setLabelShown(NodeID nid, bool val);

    /// Exposes layout info (i.e. shapes needed for shape analysis)
    const Layout &layout() const;

    /// Collapse/uncollapse a pentagon node based on its current state
    void toggleCollapsePentagon(NodeID nid);

    /// Set `nid` and its predecessors as requiring re-layout
    void dirtyUp(NodeID nid);

  signals:
    /// Triggers a redraw that updates scrollarea's viewport (perhaps a direct call would suffice)
    void needsRedrawing();

    /// Notify all views to change their current nodes to `n`
    void nodeSelected(NodeID n);

    void nogoodsClicked(std::vector<NodeID>) const;

  public slots:

    /// Update scrollarea's viewport
    void redraw();

    /// Updates the tree at 60hz (layout etc.)
    void autoUpdate();

    /// Handle double-click on a node
    void handleDoubleClick();

    /// Change zoom level
    void setScale(int scale);

    /// Center node `nid`
    void centerNode(NodeID nid);

    /// Center currently selected node
    void centerCurrentNode();

    /// Set current node to nid
    void setCurrentNode(NodeID nid);

    /// ***** NAVIGATION BEGIN *****
    /// Navigation to a node sets the node as
    /// selected and centers it in the canvas

    /// Navigate to the root
    void navRoot();
    /// Navigate to the parent of the current node
    void navUp();
    /// Navigate to the first child of the current node
    void navDown();
    /// Navigate to the last child of the current node
    void navDownAlt();
    /// Navigate to the next sibling on the left of the current node
    void navLeft();
    /// Navigate to the next sibling on the right of the current node
    void navRight();

    /// ***** NAVIGATION END *****

    /// Show/hide labels for the current node
    void toggleShowLabel();

    /// Show labels for every node below current
    void showLabelsDown();

    /// Show labels for every node on the path form root to current
    void showLabelsUp();

    /// Hide node `n` and trigger layout update
    void hideNode(NodeID n);

    /// Show/hide node (not actually used atm)
    void toggleHidden();

    /// Set current node as not hidden
    void unhideNode(NodeID nid);

    /// Mark a node and attach a note to it
    void bookmarkCurrentNode();

    /// Hide failed subtrees under node `nid`
    void hideFailedAt(NodeID nid, bool onlyDirty = false);

    /// Hide all failed descendants of the current node
    void hideFailed(bool onlyDirty = false);

    /// Unhide all nodes in the tree
    void unhideAll();

    /// Unhide all descendants of the specified node
    void unhideAllAt(NodeID n);

    /// Unhide all descendants of the current node
    void unhideAllAtCurrent();

    /// Highlight/unhighlight subtree
    void toggleHighlighted();

    /// Compute layout ignoring if it is "stale"
    void computeLayout();

    /// Set layout as stale
    void setLayoutOutdated();

    /// Print node info for debugging
    void printNodeInfo();

    /// For debugging: set all nodes on the path from the root to the current node as dirty
    void dirtyCurrentNodeUp();

    /// Show nogoods of the nodes under the current node and the node itself
    void showNogoods() const;

    /// Highlight the subtrees and hide the rest (if hide_rest is true)
    void highlightSubtrees(const std::vector<NodeID> &nodes, bool hide_rest);

    /// Transform the tree in to a lantern tree using `limit` as max lantern size
    void hideBySize(int limit);

    /// Reset lantern sizes for nodes
    void undoLanterns();

    /// Check shapes and bounding boxes of all nodes
    void debugCheckLayout() const;
};

} // namespace tree
} // namespace cpprofiler

#endif
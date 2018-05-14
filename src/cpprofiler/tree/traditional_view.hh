#ifndef CPPROFILER_TREE_TRADITIONAL_VIEW_HH
#define CPPROFILER_TREE_TRADITIONAL_VIEW_HH

#include <QWidget>

#include <memory>
#include <set>
#include "node_id.hh"
#include "visual_flags.hh"

namespace cpprofiler {
    class UserData;
}

namespace cpprofiler { namespace tree {

class Layout;
class LayoutComputer;
class NodeTree;
class NodeID;
class Structure;

class TreeScrollArea;


class TraditionalView : public QObject {
    Q_OBJECT

    const NodeTree& tree_;
    std::unique_ptr<UserData> user_data_;

    /// TODO: make sure node flags is thread-safe?
    std::unique_ptr<VisualFlags> vis_flags_;
    std::unique_ptr<Layout> layout_;

    std::unique_ptr<LayoutComputer> layout_computer_;
    std::unique_ptr<TreeScrollArea> scroll_area_;

    /// only update layout if it is stale
    bool layout_stale_ = true;

    /// returns currently selected node; can be NodeID::NoNode
    NodeID node() const;

private slots:

    void redraw();
public:

    TraditionalView(const NodeTree& tree);
    ~TraditionalView();

    QWidget* widget();

    /// Show the label for node `nid` causing layout update
    void setLabelShown(NodeID nid, bool val);

    /// Exposes layout info (i.e. shapes needed for shape analysis)
    const Layout& layout() const;

    /// Collapse/uncollapse a pentagon node based on its current state
    void toggleCollapsePentagon(NodeID nid);

    /// Set `nid` and its predecessors as requiring re-layout
    void dirtyUp(NodeID nid);

signals:
    /// Triggers a redraw that updates scrollarea's viewport (perhaps a direct call would suffice)
    void needsRedrawing();

public slots:

    /// Updates the tree at 60hz (layout etc.)
    void autoUpdate();

    /// Handle double-click on a node
    void handleDoubleClick();

    void setScale(int scale);

    void centerNode(NodeID nid);
    void centerCurrentNode();

    void navRoot();

    void navUp();
    void navDown();
    void navDownAlt();
    void navLeft();
    void navRight();

    void toggleShowLabel();
    void showLabelsDown();
    void showLabelsUp();

    void toggleHidden();
    void unhideNode(NodeID nid);

    /// Mark a node and attach a note to it
    void bookmarkCurrentNode();

    void hideFailedAt(NodeID nid, bool onlyDirty = false);

    /// hide all failed descendants of the current node
    void hideFailed();

    /// unhide all descendants of the current node
    void unhideAll();

    void toggleHighlighted();

    void selectNode(NodeID nid);

    void forceComputeLayout();

    // same as forceComputeLayout???
    void computeLayout();

    void setLayoutOutdated();

    void printNodeInfo();

    void dirtyCurrentNodeUp();

    void highlightSubtrees(const std::vector<NodeID>& nodes);


};

}}

#endif
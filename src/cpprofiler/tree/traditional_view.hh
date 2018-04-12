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

    const NodeTree& m_tree;
    std::unique_ptr<UserData> m_user_data;

    /// TODO: make sure node flags is thread-safe?
    std::unique_ptr<VisualFlags> m_vis_flags;
    std::unique_ptr<Layout> m_layout;

    std::unique_ptr<LayoutComputer> m_layout_computer;
    std::unique_ptr<TreeScrollArea> m_scroll_area;

    /// only update layout if it is stale
    bool m_layout_stale = true;

    /// returns currently selected node; can be NodeID::NoNode
    NodeID node() const;

private slots:

    void redraw();
public:

    TraditionalView(const NodeTree& tree);
    ~TraditionalView();

    QWidget* widget();

    void set_label_shown(NodeID nid, bool val);
    const Layout& layout() const;

    void toggleCollapsePentagon(NodeID nid);

    void dirtyUp(NodeID nid);

signals:
    // void nodeClicked(NodeID nid);

    void needsRedrawing();

public slots:

    void autoUpdate();

    void handleDoubleClick();

    void setScale(int scale);

    void centerNode(NodeID nid);
    void centerCurrentNode();

    void navRoot();

    void navUp();
    void navDown();
    void navLeft();
    void navRight();

    void toggleShowLabel();
    void showLabelsDown();
    void showLabelsUp();

    void toggleHidden();
    void unhideNode(NodeID nid);

    void hideFailedAt(NodeID nid);

    /// hide all failed descendants of the current node
    void hideFailed();

    /// unhide all descendants of the current node
    void unhideAll();

    void toggleHighlighted();

    void selectNode(NodeID nid);

    void forceComputeLayout();
    void computeLayout();

    void setLayoutOutdated();

    void printNodeInfo();

    void dirtyCurrentNodeUp();

    void highlight_subtrees(const std::vector<NodeID>& nodes);


};

}}

#endif
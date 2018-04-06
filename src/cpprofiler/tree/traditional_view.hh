#ifndef CPPROFILER_TREE_TRADITIONAL_VIEW_HH
#define CPPROFILER_TREE_TRADITIONAL_VIEW_HH

#include <QWidget>
#include <QAbstractScrollArea>

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

struct DisplayState {
    float scale;


    int root_x = 0;
    int root_y = 0;
};

class TreeScrollArea : public QAbstractScrollArea {
Q_OBJECT
    const NodeTree& m_tree;
    const UserData& m_user_data;
    const Layout& m_layout;

    DisplayState m_options;
    const VisualFlags& m_vis_flags;

    NodeID m_start_node;

    QPoint getNodeCoordinate(NodeID nid);
    NodeID findNodeClicked(int x, int y);

    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

signals:
    void nodeClicked(NodeID nid);
    void nodeDoubleClicked(NodeID nid);

public:

    TreeScrollArea(NodeID start,
                   const NodeTree&,
                   const UserData&,
                   const Layout&,
                   const VisualFlags&);

    /// center the x coordinate
    void centerX(int x);

    void setScale(int val);

    void changeStartNode(NodeID nid);

};


class TraditionalView : public QObject {
    Q_OBJECT

    const NodeTree& m_tree;
    std::unique_ptr<UserData> m_user_data;

    /// TODO: make sure node flags is thread-safe?
    std::unique_ptr<VisualFlags> m_vis_flags;
    std::unique_ptr<Layout> m_layout;

    std::unique_ptr<LayoutComputer> m_layout_computer;
    std::unique_ptr<TreeScrollArea> m_scroll_area;

    /// returns currently selected node; can be NodeID::NoNode
    NodeID node() const;
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

    void hideFailed();

    void toggleHighlighted();

    void selectNode(NodeID nid);

    void forceComputeLayout();

    void setLayoutOutdated();

    void printNodeInfo();

    void dirtyCurrentNodeUp();

    void highlight_subtrees(const std::vector<NodeID>& nodes);


};

}}

#endif
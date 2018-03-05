#ifndef CPPROFILER_TREE_TRADITIONAL_VIEW_HH
#define CPPROFILER_TREE_TRADITIONAL_VIEW_HH

#include <QWidget>
#include <QAbstractScrollArea>

#include <memory>
#include "node_id.hh"

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

class NodeFlags {

    std::vector<bool> m_label_shown;

    std::vector<bool> m_node_hidden;

    void ensure_id_exists(int id);

public:
    void set_label_shown(NodeID nid, bool val);
    bool get_label_shown(NodeID nid) const;

    void set_hidden(NodeID nid, bool val);
    bool get_hidden(NodeID nid) const;
};

class TreeScrollArea : public QAbstractScrollArea {
Q_OBJECT
    const NodeTree& m_tree;
    const UserData& m_user_data;
    const Layout& m_layout;

    DisplayState m_options;
    const NodeFlags& m_node_flags;


    QPoint getNodeCoordinate(NodeID nid);
    NodeID findNodeClicked(int x, int y);

    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void nodeClicked(NodeID nid);

public:

    TreeScrollArea(const NodeTree&,
                   const UserData&,
                   const Layout&,
                   const NodeFlags&);

    /// center the x coordinate
    void centerX(int x);

    void setScale(int val);

};


class TraditionalView : public QObject {
    Q_OBJECT

    const Structure& m_tree;
    std::unique_ptr<UserData> m_user_data;

    std::unique_ptr<NodeFlags> m_flags;
    std::unique_ptr<Layout> m_layout;

    std::unique_ptr<LayoutComputer> m_layout_computer;
    TreeScrollArea m_scroll_area;
public:

    TraditionalView(const NodeTree& tree);
    ~TraditionalView();

    QWidget* widget();

    const Layout& layout() const;

signals:
    // void nodeClicked(NodeID nid);

    void needsRedrawing();

public slots:
    void setScale(int scale);

    void centerNode(NodeID nid);
    void centerCurrentNode();

    void navUp();
    void navDown();
    void navLeft();
    void navRight();

    void toggleShowLabel();
    void toggleHideFailed();

    void selectNode(NodeID nid);

    void forceComputeLayout();

    void setLayoutOutdated();

    void printNodeInfo();


};

}}

#endif
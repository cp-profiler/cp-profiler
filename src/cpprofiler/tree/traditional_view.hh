#ifndef CPPROFILER_TREE_TRADITIONAL_VIEW_HH
#define CPPROFILER_TREE_TRADITIONAL_VIEW_HH

#include <QWidget>
#include <QAbstractScrollArea>

#include <memory>

namespace cpprofiler {
    class UserData;
}

namespace cpprofiler { namespace tree {

class Layout;
class NodeTree;

class TreeScrollArea : public QAbstractScrollArea {

    const NodeTree& m_tree;
    const UserData& m_user_data;
    const Layout& m_layout;

    void paintEvent(QPaintEvent* e) override;

public:

    TreeScrollArea(const NodeTree& tree, const UserData& user_data, const Layout& layout);

};

class TraditionalView {

    std::unique_ptr<Layout> m_layout;

    TreeScrollArea m_scroll_area;



public:

    TraditionalView(const NodeTree& tree, const UserData& user_data);
    ~TraditionalView();

    QWidget* widget();

};

}}

#endif
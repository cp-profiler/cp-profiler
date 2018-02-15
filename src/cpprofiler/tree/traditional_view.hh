#ifndef CPPROFILER_TREE_TRADITIONAL_VIEW_HH
#define CPPROFILER_TREE_TRADITIONAL_VIEW_HH

#include <QWidget>
#include <QAbstractScrollArea>

#include <memory>

namespace cpprofiler { namespace tree {

class Layout;
class Structure;

class TreeScrollArea : public QAbstractScrollArea {

    const Structure& m_tree;
    const Layout& m_layout;

    void paintEvent(QPaintEvent* e) override;

public:

    TreeScrollArea(const Structure& tree, const Layout& layout);

};

class TraditionalView {

    // TreeWidget m_widget;

    const Structure& m_tree;
    std::unique_ptr<Layout> m_layout;

    TreeScrollArea m_scroll_area;



public:

    TraditionalView(const Structure& tree);
    ~TraditionalView();

    QWidget* widget();

};

}}

#endif
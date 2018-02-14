#ifndef CPPROFILER_TREE_TRADITIONAL_VIEW_HH
#define CPPROFILER_TREE_TRADITIONAL_VIEW_HH

#include <QWidget>
#include <QAbstractScrollArea>

#include <memory>

namespace cpprofiler { namespace tree {

class Layout;
class Structure;

class TreeScrollArea : public QAbstractScrollArea {

    void paintEvent(QPaintEvent* e) override;


};

class TraditionalView {

    // TreeWidget m_widget;

    const Structure& m_tree;

    TreeScrollArea m_scroll_area;

    std::unique_ptr<Layout> m_layout;


public:

    TraditionalView(const Structure& nt);
    ~TraditionalView();

    QWidget* widget();

};

}}

#endif
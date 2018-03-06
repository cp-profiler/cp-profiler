#pragma once

#include <QWidget>

#include "../user_data.hh"
#include "traditional_view.hh"
#include "layout_computer.hh"
#include "layout.hh"
#include "../core.hh"

namespace cpprofiler { namespace tree {


class SubtreeView : public QWidget {
Q_OBJECT

    std::unique_ptr<UserData> m_user_data;
    std::unique_ptr<Layout> m_layout;
    std::unique_ptr<TreeScrollArea> m_scroll_area;

    /// Not really used in this view?
    NodeFlags node_flags;

    NodeID m_cur_node = NodeID::NoNode;

public:
    SubtreeView(const NodeTree& nt)
    :   m_user_data(utils::make_unique<UserData>())
    ,   m_layout(utils::make_unique<Layout>())
    {
        auto layout_computer = LayoutComputer(nt, *m_layout, node_flags);
        layout_computer.compute();

        m_scroll_area.reset(new TreeScrollArea(m_cur_node, nt, *m_user_data, *m_layout, node_flags));
        m_scroll_area->setScale(50);
        m_scroll_area->viewport()->update();
    }

    QWidget* widget() {
        return m_scroll_area.get();
    }

public slots:

    void setNode(NodeID nid) {
        m_cur_node = nid;
        m_scroll_area->changeStartNode(nid);
        m_scroll_area->viewport()->update();
    }

};

}}
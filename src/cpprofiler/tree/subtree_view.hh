#pragma once

#include <QWidget>

#include "../user_data.hh"
#include "traditional_view.hh"
#include "layout_computer.hh"
#include "../core.hh"

namespace cpprofiler { namespace tree {




class SubtreeView : public QWidget {

    std::unique_ptr<UserData> m_user_data;
    std::unique_ptr<Layout> m_layout;
    std::unique_ptr<TreeScrollArea> m_scroll_area;


    /// Not really used in this view?
    NodeFlags node_flags;

public:
    SubtreeView(const NodeTree& nt)
    :   m_user_data(utils::make_unique<UserData>())
    ,   m_layout(utils::make_unique<Layout>())
    {
        auto layout_computer = LayoutComputer(nt, *m_layout, node_flags);
        layout_computer.compute();

        /// TODO: come back here (why not displayed?)
        m_scroll_area.reset(new TreeScrollArea(nt, *m_user_data, *m_layout, node_flags));

        m_scroll_area->viewport()->update();
    }

    QWidget* widget() {
        return m_scroll_area.get();
    }

};

}}
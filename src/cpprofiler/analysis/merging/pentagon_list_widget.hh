#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QVBoxLayout>

#include "merge_result.hh"

#include "pentagon_rect.hh"

namespace cpprofiler { namespace analysis {


class PentagonListWidget : public QWidget {
Q_OBJECT

    QGraphicsView* m_view;
    QGraphicsScene* m_scene;

    const MergeResult& m_merge_res;

    bool needs_sorting = false;

    /// Which pentagon from the list is selected
    NodeID m_selected = NodeID::NoNode;


public:

    PentagonListWidget(QWidget* w, const MergeResult& res) : QWidget(w), m_merge_res(res) {

        auto layout = new QVBoxLayout(this);

        m_view = new QGraphicsView(this);
        m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        m_view->setMaximumWidth(pent_config::VIEW_WIDTH);
        m_view->setMinimumWidth(pent_config::VIEW_WIDTH);

        m_scene = new QGraphicsScene(this);
        m_view->setScene(m_scene);

        layout->addWidget(m_view);
    }

    /// Width available for drawing
    int viewWidth() {
        return m_view->viewport()->width();
    }

    void handleClick(NodeID node) {

        m_selected = node;
        emit pentagonClicked(node);
        updateScene();
    }

    void updateScene() {
        using namespace pent_config;

        m_scene->clear();

        auto ypos = 2;

        /// This is used to scale the horizontal bars
        int max_value = 0;
        for (const auto& pen : m_merge_res) {
            max_value = std::max(max_value, std::max(pen.size_l, pen.size_r));
        }

        auto displayed_items = m_merge_res; /// make copy

        if (needs_sorting) {
            std::sort(displayed_items.begin(), displayed_items.end(),
                [](const PentagonItem& p1, const PentagonItem& p2) {
                    return std::abs(p1.size_r-p1.size_l) > std::abs(p2.size_r-p2.size_l);
                });
        }

        for (auto i = 0; i < displayed_items.size(); ++i) {
            auto ypos = i * (HEIGHT + PADDING) + PADDING;

            bool sel = (displayed_items[i].pen_nid == m_selected);
            new PentagonRect(m_scene, *this, displayed_items[i], ypos, max_value, sel);
        }
    }

signals:
    void pentagonClicked(NodeID);


public slots:
    void handleSortCB(int state) {
        if (state == Qt::Checked) {
            needs_sorting = true;
        } else {
            needs_sorting = false;
        }

        updateScene();
    }


};






}}
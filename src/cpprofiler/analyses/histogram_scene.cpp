#include "histogram_scene.hh"

namespace cpprofiler { namespace analysis {

QColor PatternRect::normal_outline{252, 209, 22};
QColor PatternRect::highlighted_outline{252, 148, 77};

static PatternRect* addRect(HistogramScene& hist_scene, int row, int val, SubtreePattern&& pattern) {

    int x = 0;
    int y = row * ROW_HEIGHT;

    int width = val * 40;

    /// Note: items will be deleted when the scene is destroyed
    auto item = new PatternRect{hist_scene, x, y, width, SHAPE_RECT_HEIGHT, std::move(pattern)};
    item->addToScene();

    return item;
}

static void addText(QGraphicsScene& scene, int col, int row, const char* text) {
    auto str = new QGraphicsSimpleTextItem{text};
    addText(scene, col, row, str);
}

static void addText(QGraphicsScene& scene, int col, int row, int value) {
    auto int_text_item = new QGraphicsSimpleTextItem{QString::number(value)};
    addText(scene, col, row, int_text_item, Align::RIGHT);
}

void HistogramScene::drawPatterns(std::vector<SubtreePattern>&& patterns) {

    auto& scene = *m_scene.get();

    addText(scene, 0, 0, "hight");
    addText(scene, 1, 0, "count");
    addText(scene, 2, 0, "size");

    int row = 1;

    for (auto&& pattern : patterns) {

        const auto nid = pattern.first();
        const int count = pattern.count();
        const int height = pattern.height();

        auto item = addRect(*this, row, count, std::move(pattern));

        m_rects.push_back(item);

        addText(scene, 0, row, height);
        addText(scene, 1, row, count);
        addText(scene, 2, row, "?");

        ++row;
    }
}

int HistogramScene::findPatternIdx(PatternRect* pattern) {
    for (auto i = 0; i < m_rects.size(); ++i) {
        if (pattern == m_rects[i]) {
            return i;
        }
    }

    return -1;
}

void HistogramScene::setPatternSelected(int idx) {

    if (idx < 0 || idx >= m_rects.size()) return;

    auto prect = m_rects[idx];

    emit pattern_selected(prect->node());

    emit should_be_highlighted(prect->nodes());

    {
        if (idx == m_selected_idx) {
            m_selected->set_highlighted(false);
            m_selected_idx = -1;
            m_selected = nullptr;
        } else {
            m_selected_idx = idx;
            /// unselect the old one
            if (m_selected) {
                m_selected->set_highlighted(false);
            }
            prect->set_highlighted(true);
            m_selected_idx = idx;
            m_selected = prect;
        }
    }


}

void HistogramScene::handleClick(PatternRect* prect) {

    auto idx = findPatternIdx(prect);

    setPatternSelected(idx);
}

void HistogramScene::prevPattern() {
    if (m_rects.size() == 0) return;

    auto new_idx = m_selected_idx - 1;

    if (new_idx < 0) return;

    setPatternSelected(new_idx);
}

void HistogramScene::nextPattern() {
    if (m_rects.size() == 0) return;

    auto new_idx = m_selected_idx + 1;

    if (new_idx >= m_rects.size()) return;

    setPatternSelected(new_idx);

}


}}
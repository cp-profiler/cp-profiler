#include "histogram_scene.hh"
#include "pattern_rect.hh"

namespace cpprofiler
{
namespace analysis
{

QColor PatternRect::normal_outline{252, 209, 22};
QColor PatternRect::highlighted_outline{252, 148, 77};

static std::shared_ptr<PatternRect> addRect(HistogramScene &hist_scene, int row, int val)
{

    int x = 0;
    int y = row * ROW_HEIGHT;

    int width = val * 40;

    /// Note: items will be deleted when the scene is destroyed
    auto item = std::make_shared<PatternRect>(hist_scene, x, y, width, SHAPE_RECT_HEIGHT);
    item->addToScene();

    return item;
}

static void addText(QGraphicsScene &scene, int col, int row, const char *text)
{
    auto str = new QGraphicsSimpleTextItem{text};
    addText(scene, col, row, str);
}

static void addText(QGraphicsScene &scene, int col, int row, int value)
{
    auto int_text_item = new QGraphicsSimpleTextItem{QString::number(value)};
    addText(scene, col, row, int_text_item, Align::RIGHT);
}

void HistogramScene::drawPatterns()
{

    auto &scene = *m_scene;

    addText(scene, 0, 0, "hight");
    addText(scene, 1, 0, "count");
    addText(scene, 2, 0, "size");

    int row = 1;

    for (auto &&p : patterns_)
    {

        const auto nid = p->first();
        const int count = p->count();
        const int height = p->height();

        auto item = addRect(*this, row, count);

        m_rects.push_back(item);
        rect2pattern_.insert(std::make_pair(item, p));

        /// number of nodes in the frist subtree represeting a pattern
        const auto size = p->size_;

        addText(scene, 0, row, height);
        addText(scene, 1, row, count);
        addText(scene, 2, row, size);

        ++row;
    }
}

void HistogramScene::setPatterns(std::vector<SubtreePattern> &&patterns)
{
    patterns_.reserve(patterns.size());

    for (auto &p : patterns)
    {
        patterns_.push_back(std::make_shared<SubtreePattern>(std::move(p)));
    }
}

int HistogramScene::findPatternIdx(PatternRect *pattern)
{
    for (auto i = 0; i < m_rects.size(); ++i)
    {
        if (pattern == m_rects[i].get())
        {
            return i;
        }
    }

    return -1;
}

PatternPtr HistogramScene::rectToPattern(PatternRectPtr prect)
{
    if (rect2pattern_.find(prect) != rect2pattern_.end())
    {
        return rect2pattern_[prect];
    }
    return nullptr;
}

void HistogramScene::reset()
{

    if (m_selected)
    {
        m_selected->setHighlighted(false);
        m_selected = nullptr;
        m_selected_idx = -1;
    }

    m_rects.clear();
    rect2pattern_.clear();
    m_scene->clear();
    patterns_.clear();
}

void HistogramScene::setPatternSelected(int idx)
{

    if (idx < 0 || idx >= m_rects.size())
        return;

    auto prect = m_rects[idx];

    auto pattern = rectToPattern(prect);
    if (!pattern)
        return;

    emit pattern_selected(pattern->first());

    emit should_be_highlighted(pattern->nodes());

    {
        if (idx == m_selected_idx)
        {
            m_selected->setHighlighted(false);
            m_selected_idx = -1;
            m_selected = nullptr;
        }
        else
        {
            m_selected_idx = idx;
            /// unselect the old one
            if (m_selected)
            {
                m_selected->setHighlighted(false);
            }
            prect->setHighlighted(true);
            m_selected_idx = idx;
            m_selected = prect.get();
        }
    }
}

void HistogramScene::handleClick(PatternRect *prect)
{

    auto idx = findPatternIdx(prect);

    setPatternSelected(idx);
}

void HistogramScene::prevPattern()
{
    if (m_rects.size() == 0)
        return;

    auto new_idx = m_selected_idx - 1;

    if (new_idx < 0)
        return;

    setPatternSelected(new_idx);
}

void HistogramScene::nextPattern()
{
    if (m_rects.size() == 0)
        return;

    auto new_idx = m_selected_idx + 1;

    if (new_idx >= m_rects.size())
        return;

    setPatternSelected(new_idx);
}

} // namespace analysis
} // namespace cpprofiler
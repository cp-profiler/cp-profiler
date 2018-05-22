#ifndef CPPROFILER_ANALYSES_HISTOGRAM_SCENE
#define CPPROFILER_ANALYSES_HISTOGRAM_SCENE

#include <memory>
#include <unordered_map>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>

#include "../core.hh"
#include "subtree_pattern.hh"

namespace cpprofiler
{
namespace analysis
{

static constexpr int SHAPE_RECT_HEIGHT = 16;
static constexpr int NUMBER_WIDTH = 50;
static constexpr int COLUMN_WIDTH = NUMBER_WIDTH + 10;

/// vertical distance between two shape rectangles
static constexpr int V_DISTANCE = 2;
static constexpr int ROW_HEIGHT = SHAPE_RECT_HEIGHT + V_DISTANCE;

enum class Align
{
    CENTER,
    RIGHT
};

static void addText(QGraphicsScene &scene, int col, int row,
                    QGraphicsSimpleTextItem *text_item,
                    Align alignment = Align::CENTER)
{
    int item_width = text_item->boundingRect().width();
    int item_height = text_item->boundingRect().height();

    // center the item vertically at y
    int y_offset = item_height / 2;
    int x_offset = 0;

    switch (alignment)
    {
    case Align::CENTER:
        x_offset = (COLUMN_WIDTH - item_width) / 2;
        break;
    case Align::RIGHT:
        x_offset = COLUMN_WIDTH - item_width;
        break;
    }

    int x = col * COLUMN_WIDTH + x_offset;
    int y = row * ROW_HEIGHT + y_offset;

    text_item->setPos(x, y - y_offset);
    scene.addItem(text_item);
}

class PatternRect;

using PatternRectPtr = std::shared_ptr<PatternRect>;
using PatternPtr = std::shared_ptr<SubtreePattern>;

class HistogramScene : public QObject
{
    Q_OBJECT
    std::unique_ptr<QGraphicsScene> m_scene;

    PatternRect *m_selected = nullptr;
    int m_selected_idx = -1;

    /// need a list of rects to enable navigation;
    std::vector<PatternRectPtr> m_rects;

    std::vector<PatternPtr> patterns_;

    std::unordered_map<PatternRectPtr, PatternPtr> rect2pattern_;

    /// find pattern idx in m_rects using PatternRect
    int findPatternIdx(PatternRect *pattern);

    void setPatternSelected(int idx);

    PatternPtr rectToPattern(PatternRectPtr prect);

  public:
    HistogramScene()
    {

        m_scene.reset(new QGraphicsScene());
    }

    void reset();

    QGraphicsScene *scene()
    {
        return m_scene.get();
    }

    void handleClick(PatternRect *prect);

    /// Draw the patterns onto the scene
    void drawPatterns();

    void setPatterns(std::vector<SubtreePattern> &&patterns);

  signals:

    void pattern_selected(NodeID);
    void should_be_highlighted(const std::vector<NodeID> &);

  public slots:

    void prevPattern();
    void nextPattern();
};

} // namespace analysis
} // namespace cpprofiler

#endif
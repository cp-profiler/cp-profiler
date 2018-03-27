#ifndef CPPROFILER_ANALYSES_HISTOGRAM_SCENE
#define CPPROFILER_ANALYSES_HISTOGRAM_SCENE

#include <memory>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>

#include "../core.hh"
#include "subtree_pattern.hh"

namespace cpprofiler { namespace analysis {

static constexpr int SHAPE_RECT_HEIGHT = 16;
static constexpr int NUMBER_WIDTH = 50;
static constexpr int COLUMN_WIDTH = NUMBER_WIDTH + 10;
static constexpr int ROW_HEIGHT = SHAPE_RECT_HEIGHT + 2;

static constexpr int SELECTION_WIDTH = 500;

enum class Align {
  CENTER,
  RIGHT
};

static void addText(QGraphicsScene& scene, int col, int row,
                    QGraphicsSimpleTextItem* text_item,
                    Align alignment = Align::CENTER) {
    int item_width = text_item->boundingRect().width();
    int item_height = text_item->boundingRect().height();

    // center the item vertically at y
    int y_offset = item_height / 2;
    int x_offset = 0;

    switch (alignment) {
    case Align::CENTER:
        x_offset = (COLUMN_WIDTH - item_width) / 2; break;
    case Align::RIGHT:
        x_offset = COLUMN_WIDTH - item_width; break;
    }

    int x = col * COLUMN_WIDTH + x_offset;
    int y = row * ROW_HEIGHT   + y_offset;

    text_item->setPos(x, y - y_offset);
    scene.addItem(text_item);
}

class PatternRect;

class HistogramScene : public QObject {
Q_OBJECT
    std::unique_ptr<QGraphicsScene> m_scene;

    PatternRect* m_selected = nullptr;
    int m_selected_idx = -1;

    /// need a list of rects to enable navigation;
    std::vector<PatternRect*> m_rects;

    /// find pattern idx in m_rects using PatternRect
    int findPatternIdx(PatternRect* pattern);

    void setPatternSelected(int idx);

public:
    HistogramScene() {

        m_scene.reset(new QGraphicsScene());

    }

    QGraphicsScene* scene() {
        return m_scene.get();
    }

    void reset() {
        m_scene.reset(nullptr);
        m_rects.clear();
    }


    void handleClick(PatternRect* prect);

    void drawPatterns(std::vector<SubtreePattern>&& patterns);

signals:

    void pattern_selected(NodeID);
    void should_be_highlighted(const std::vector<NodeID>&);

public slots:

    void prevPattern();
    void nextPattern();

};

class PatternRect : public QGraphicsRectItem {

    SubtreePattern m_pattern;

    QGraphicsRectItem visible_rect;

    HistogramScene& m_hist_scene;

    void mousePressEvent(QGraphicsSceneMouseEvent*) override {
        m_hist_scene.handleClick(this);
    }

    static QColor normal_outline;
    static QColor highlighted_outline;

public:
    PatternRect(HistogramScene& hist_scene, int x, int y, int width, int height, SubtreePattern&& pattern)
    : QGraphicsRectItem(x, y, SELECTION_WIDTH, height),
      m_pattern(std::move(pattern)), visible_rect(x, y, width, height), m_hist_scene(hist_scene) {

        //   setFlag(QGraphicsItem::ItemIsSelectable);

        QColor gold(252, 209, 22);
        setPen(Qt::NoPen);

        QColor patternRectColor(255, 215, 179);
        visible_rect.setBrush(patternRectColor);
      }

    void set_highlighted(bool val) {

        QPen pen;
        if (val) {
            pen.setWidth(3);
            pen.setBrush(highlighted_outline);
        } else {
            // pen.setBrush(normal_outline);
            pen.setStyle(Qt::NoPen);
        }
        setPen(pen);
    }

    void addToScene() {
        m_hist_scene.scene()->addItem(&visible_rect);
        m_hist_scene.scene()->addItem(this);
    }

    NodeID node() {
        return m_pattern.first();
    }

    const std::vector<NodeID>& nodes() {
        return m_pattern.nodes();
    }

};





}}

#endif
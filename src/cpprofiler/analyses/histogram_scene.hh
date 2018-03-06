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

    void drawPattern() {

    }

public:
    HistogramScene() {

        m_scene.reset(new QGraphicsScene());

    }

    QGraphicsScene* scene() {
        return m_scene.get();
    }

    void reset() {
        m_scene.reset(nullptr);
    }


    void handleClick(PatternRect* prect);

    void drawPatterns(std::vector<SubtreePattern>&& patterns);

signals:

    void pattern_selected(NodeID);
    void should_be_highlighted(const std::vector<NodeID>&);

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
          setPen(gold);
      }

    void set_highlighted(bool val) {
        if (val) {
            setPen(highlighted_outline);
        } else {
            setPen(normal_outline);
        }
    }

    void addToScene() {
        m_hist_scene.scene()->addItem(this);
        m_hist_scene.scene()->addItem(&visible_rect);
    }

    NodeID node() {
        return m_pattern.first();
    }

    const std::vector<NodeID>& nodes() {
        return m_pattern.nodes();
    }

};

inline QColor PatternRect::normal_outline{252, 209, 22};
inline QColor PatternRect::highlighted_outline{252, 22, 22};



static void addRect(HistogramScene& hist_scene, int row, int val, SubtreePattern&& pattern) {

    int x = 0;
    int y = row * ROW_HEIGHT;

    int width = val * 40;

    /// Note: items will be deleted when the scene is destroyed
    auto item = new PatternRect{hist_scene, x, y, width, SHAPE_RECT_HEIGHT, std::move(pattern)};
    item->addToScene();
}

static void addText(QGraphicsScene& scene, int col, int row, const char* text) {
    auto str = new QGraphicsSimpleTextItem{text};
    addText(scene, col, row, str);
}

static void addText(QGraphicsScene& scene, int col, int row, int value) {
    auto int_text_item = new QGraphicsSimpleTextItem{QString::number(value)};
    addText(scene, col, row, int_text_item, Align::RIGHT);
}

inline void HistogramScene::drawPatterns(std::vector<SubtreePattern>&& patterns) {

    auto& scene = *m_scene.get();

    addText(scene, 0, 0, "hight");
    addText(scene, 1, 0, "count");
    addText(scene, 2, 0, "size");

    int row = 1;

    for (auto&& pattern : patterns) {

        const auto nid = pattern.first();
        const int count = pattern.count();
        const int height = pattern.height();

        addRect(*this, row, count, std::move(pattern));

        addText(scene, 0, row, height);
        addText(scene, 1, row, count);
        addText(scene, 2, row, "?");

        ++row;
    }
}

inline void HistogramScene::handleClick(PatternRect* prect) {


    emit pattern_selected(prect->node());

    emit should_be_highlighted(prect->nodes());

    {
        if (prect == m_selected) {
            m_selected->set_highlighted(false);
            m_selected = nullptr;
        } else {
            if (m_selected) {
                m_selected->set_highlighted(false);
            }
            prect->set_highlighted(true);
            m_selected = prect;
        }

    }
}



}}

#endif
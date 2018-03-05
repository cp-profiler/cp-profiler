#include "similar_subtree_window.hh"

#include <QGraphicsView>
#include <QVBoxLayout>
#include <QSplitter>
#include <QGraphicsSimpleTextItem>
#include <QDebug>


#include <set>

#include "../tree/shape.hh"
#include "../tree/structure.hh"
#include "../tree/layout.hh"
#include "../tree/node_tree.hh"

#include "histogram_scene.hh"

#include "../tree/subtree_view.hh"

using namespace cpprofiler::tree;

namespace cpprofiler { namespace analysis {

SimilarSubtreeWindow::SimilarSubtreeWindow(QWidget* parent, const tree::NodeTree& nt, const tree::Layout& lo)
: QDialog(parent), m_nt(nt), m_lo(lo) {

    m_histogram.reset(new HistogramScene);
    auto hist_view = new QGraphicsView{this};

    // todo: remove when parent is deleted
    // todo: show the visualisation
    auto subtree_view = new SubtreeView{nt};

    hist_view->setScene(m_histogram->scene());


    auto splitter = new QSplitter{this};

    splitter->addWidget(hist_view);
    splitter->addWidget(subtree_view->widget());
    splitter->setSizes(QList<int>{1, 1});

    auto globalLayout = new QVBoxLayout{this};
    globalLayout->addWidget(splitter, 1);

    analyse();


}

struct ShapeInfo {
    NodeID nid;
    const Shape& shape;
};

struct CompareShapes {
 public:
  bool operator()(const ShapeInfo& si1, const ShapeInfo& si2) const {

    const auto& s1 = si1.shape;
    const auto& s2 = si2.shape;

    if (s1.depth() < s2.depth()) return true;
    if (s1.depth() > s2.depth()) return false;

    for (int i = 0; i < s1.depth(); ++i) {
      if (s1[i].l < s2[i].l) return false;
      if (s1[i].l > s2[i].l) return true;
      if (s1[i].r < s2[i].r) return true;
      if (s1[i].r > s2[i].r) return false;
    }
    return false;
  }
};

struct ShapePattern {
    std::vector<NodeID> nodes;
    int m_height;

    int count() {
        return nodes.size();
    }

    int height() {
        return m_height;
    }

    NodeID first() {
        return nodes.at(0);
    }
};

static std::vector<ShapePattern> runSimilarShapes(const Structure& tree, const Layout& lo) {

    std::multiset<ShapeInfo, CompareShapes> shape_set;

    auto node_order = helper::postOrder_unsafe(tree);

    for (auto nid : node_order) {
        shape_set.insert({nid, lo.getShape_unsafe(nid)});
    }

    std::vector<ShapePattern> shapes;

    auto it = shape_set.begin(); auto end = shape_set.end();

    while (it != end) {
        auto upper = shape_set.upper_bound(*it);

        const int height = it->shape.depth();
        shapes.push_back(ShapePattern{{}, height});
        for (; it != upper; ++it) {
            shapes[shapes.size() - 1].nodes.push_back(it->nid);
        }
    }

    return shapes;

}


void SimilarSubtreeWindow::analyse() {


    // analyse_shapes();

    /// TODO: make sure building is finished


    auto shapes = runSimilarShapes(m_nt.tree_structure(), m_lo);


    m_histogram->drawPatterns<ShapePattern>(shapes);

    // m_histogram->reset();


}


// void SimilarSubtreeWindow::analyse_shapes() {

// }

}}
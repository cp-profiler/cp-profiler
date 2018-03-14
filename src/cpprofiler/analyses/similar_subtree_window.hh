
#ifndef CPPROFILER_ANALYSIS_SIMILAR_SUBTREE_WINDOW_HH
#define CPPROFILER_ANALYSIS_SIMILAR_SUBTREE_WINDOW_HH

#include <QDialog>
#include <memory>

#include "../core.hh"

class QGraphicsScene;

namespace cpprofiler { namespace tree {
    class NodeTree;
    class Layout;
    class SubtreeView;
}}

namespace cpprofiler { namespace analysis {


enum class SimilarityType {
  SUBTREE, SHAPE
};

class HistogramScene;

class SimilarSubtreeWindow : public QDialog {
Q_OBJECT
private:

    // void analyse_shapes();

    const tree::NodeTree& m_nt;
    const tree::Layout& m_lo;

    std::unique_ptr<HistogramScene> m_histogram;

    std::unique_ptr<tree::SubtreeView> m_subtree_view;

    SimilarityType m_sim_type = SimilarityType::SUBTREE;

public:
    SimilarSubtreeWindow(QWidget* parent, const tree::NodeTree& nt, const tree::Layout& lo);

    ~SimilarSubtreeWindow();

    void analyse();

signals:

    void should_be_highlighted(const std::vector<NodeID>& nodes);

};


}}


#endif
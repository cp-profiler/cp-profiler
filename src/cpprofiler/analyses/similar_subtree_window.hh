
#ifndef CPPROFILER_ANALYSIS_SIMILAR_SUBTREE_WINDOW_HH
#define CPPROFILER_ANALYSIS_SIMILAR_SUBTREE_WINDOW_HH

#include <QDialog>
#include <memory>

class QGraphicsScene;

namespace cpprofiler { namespace tree {
    class NodeTree;
    class Layout;
}}

namespace cpprofiler { namespace analysis {

class HistogramScene;

class SimilarSubtreeWindow : public QDialog {

private:

    // void analyse_shapes();

    const tree::NodeTree& m_nt;
    const tree::Layout& m_lo;

    std::unique_ptr<HistogramScene> m_histogram;

public:
    SimilarSubtreeWindow(QWidget* parent, const tree::NodeTree& nt, const tree::Layout& lo);

    void analyse();



};


}}


#endif
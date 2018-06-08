
#ifndef CPPROFILER_ANALYSIS_SIMILAR_SUBTREE_WINDOW_HH
#define CPPROFILER_ANALYSIS_SIMILAR_SUBTREE_WINDOW_HH

#include <QDialog>
#include <QLineEdit>
#include <memory>

#include "../core.hh"

class QGraphicsScene;

namespace cpprofiler
{
namespace tree
{
class NodeTree;
class Layout;
class SubtreeView;
} // namespace tree
} // namespace cpprofiler

namespace cpprofiler
{
namespace analysis
{

enum class SimilarityType
{
    SUBTREE,
    SHAPE
};

class HistogramScene;

/// Text line displaying difference on the path for two subtrees
class PathDiffLine : public QLineEdit
{
  public:
    PathDiffLine() : QLineEdit()
    {
        setReadOnly(true);
    }
};

class SimilarSubtreeWindow : public QDialog
{
    Q_OBJECT
  private:
    // void analyse_shapes();

    const tree::NodeTree &m_nt;
    std::unique_ptr<tree::Layout> m_lo;

    std::unique_ptr<HistogramScene> m_histogram;

    std::unique_ptr<tree::SubtreeView> m_subtree_view;

    /// Text line displaying difference on the path for two subtrees of a pattern
    PathDiffLine path_line1_;
    PathDiffLine path_line2_;

    // SimilarityType m_sim_type = SimilarityType::SUBTREE;
    SimilarityType m_sim_type = SimilarityType::SHAPE;

  public:
    SimilarSubtreeWindow(QWidget *parent, const tree::NodeTree &nt);

    ~SimilarSubtreeWindow();

    void analyse();

  signals:

    void should_be_highlighted(const std::vector<NodeID> &nodes);

  public slots:
    /// Calculate the difference in label paths for the first two nodes
    void updatePathDiff(const std::vector<NodeID> &nodes);
};

} // namespace analysis
} // namespace cpprofiler

#endif
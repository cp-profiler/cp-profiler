#include "similar_subtree_window.hh"

#include <QGraphicsView>
#include <QVBoxLayout>
#include <QSplitter>
#include <QGraphicsSimpleTextItem>
#include <QDebug>
#include <QLabel>
#include <QComboBox>
#include <QAction>

#include <iostream>
#include <algorithm>

#include "../tree/shape.hh"
#include "../tree/structure.hh"
#include "../tree/layout.hh"
#include "../tree/node_tree.hh"
#include "../utils/tree_utils.hh"

#include "subtree_pattern.hh"
#include "similar_subtree_analysis.hh"
#include "../tree/subtree_view.hh"

#include "histogram_scene.hh"
#include "../utils/perf_helper.hh"

using namespace cpprofiler::tree;

using std::vector;

namespace cpprofiler
{
namespace analysis
{

SimilarSubtreeWindow::SimilarSubtreeWindow(QWidget *parent, const tree::NodeTree &nt)
    : QDialog(parent), m_nt(nt)
{

    m_histogram.reset(new HistogramScene);
    auto hist_view = new QGraphicsView{this};

    hist_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_subtree_view.reset(new SubtreeView{nt});

    hist_view->setScene(m_histogram->scene());

    auto globalLayout = new QVBoxLayout{this};

    {
        auto settingsLayout = new QHBoxLayout{};
        globalLayout->addLayout(settingsLayout);
        settingsLayout->addWidget(new QLabel{"Similarity Criteria:"}, 0, Qt::AlignRight);

        auto typeChoice = new QComboBox();
        typeChoice->addItems({"contour", "subtree"});
        settingsLayout->addWidget(typeChoice);

        connect(typeChoice, &QComboBox::currentTextChanged, [this](const QString &str) {
            if (str == "contour")
            {
                m_sim_type = SimilarityType::SHAPE;
            }
            else if (str == "subtree")
            {
                m_sim_type = SimilarityType::SUBTREE;
            }
            analyse();
        });
    }

    auto splitter = new QSplitter{this};

    splitter->addWidget(hist_view);
    splitter->addWidget(m_subtree_view->widget());
    splitter->setSizes(QList<int>{1, 1});

    globalLayout->addWidget(splitter, 1);

    globalLayout->addWidget(&path_line1_);
    globalLayout->addWidget(&path_line2_);

    analyse();

    /// see if I can add action here

    auto keyDown = new QAction{"Press down", this};
    keyDown->setShortcuts({QKeySequence("Down"), QKeySequence("J")});
    addAction(keyDown);

    connect(keyDown, &QAction::triggered, m_histogram.get(), &HistogramScene::nextPattern);

    auto keyUp = new QAction{"Press up", this};
    keyUp->setShortcuts({QKeySequence("Up"), QKeySequence("K")});
    addAction(keyUp);

    connect(keyUp, &QAction::triggered, m_histogram.get(), &HistogramScene::prevPattern);

    connect(keyUp, &QAction::triggered, [this]() {
        update();
    });
}

SimilarSubtreeWindow::~SimilarSubtreeWindow() = default;

using std::vector;

std::ostream &operator<<(std::ostream &out, const Group &group)
{

    const auto size = group.size();
    if (size == 0)
    {
        return out;
    }
    for (auto i = 0; i < size - 1; ++i)
    {
        out << (int)group[i] << " ";
    }
    if (size > 0)
    {
        out << (int)group[size - 1];
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const vector<Group> &groups)
{

    const auto size = groups.size();

    if (size == 0)
        return out;
    for (auto i = 0; i < size - 1; ++i)
    {
        out << groups[i];
        out << "|";
    }

    out << groups[size - 1];

    return out;
}

std::ostream &operator<<(std::ostream &out, const Partition &partition)
{

    out << "[";
    out << partition.processed;
    out << "] [";
    out << partition.remaining;
    out << "]";

    return out;
}

static vector<SubtreePattern> eliminateSubsumed(const NodeTree &tree,
                                                const vector<SubtreePattern> &patterns)
{

    std::set<NodeID> marked;

    for (const auto &pattern : patterns)
    {
        for (auto nid : pattern.nodes())
        {

            const auto kids = tree.childrenCount(nid);
            for (auto alt = 0; alt < kids; ++alt)
            {
                auto kid = tree.getChild(nid, alt);
                marked.insert(kid);
            }
        }
    }

    vector<SubtreePattern> result;

    for (const auto &pattern : patterns)
    {

        const auto &nodes = pattern.nodes();

        /// Determine if all nodes are in 'marked':
        auto subset = true;

        for (auto nid : nodes)
        {
            if (marked.find(nid) == marked.end())
            {
                subset = false;
                break;
            }
        }

        if (!subset)
        {
            result.push_back(pattern);
        }
    }

    return result;
}

static std::unique_ptr<tree::Layout> computeShapes(const NodeTree &tree)
{
    auto layout = std::unique_ptr<tree::Layout>(new Layout);
    tree::VisualFlags vf;
    tree::LayoutComputer layout_c(tree, *layout, vf);
    layout_c.compute();
    return std::move(layout);
}

void SimilarSubtreeWindow::analyse()
{

    // analyse_shapes();

    /// TODO: make sure building is finished

    auto patterns = std::vector<SubtreePattern>{};

    switch (m_sim_type)
    {
    case SimilarityType::SUBTREE:
    {
        patterns = runIdenticalSubtrees(m_nt);
    }
    break;
    case SimilarityType::SHAPE:
    {
        if (!m_lo)
        {
            m_lo = computeShapes(m_nt);
        }
        patterns = runSimilarShapes(m_nt, *m_lo);
    }
    }

    /// make sure there are only patterns of cardinality > 1

    auto new_end = std::remove_if(patterns.begin(), patterns.end(), [](const SubtreePattern &pattern) {
        return pattern.count() <= 1;
    });

    patterns.resize(std::distance(patterns.begin(), new_end));

    patterns = eliminateSubsumed(m_nt, patterns);

    perfHelper.begin("set patterns");

    m_histogram->reset();
    m_histogram->setPatterns(std::move(patterns));
    m_histogram->drawPatterns();

    perfHelper.end();

    connect(m_histogram.get(), &HistogramScene::should_be_highlighted,
            this, &SimilarSubtreeWindow::should_be_highlighted);

    connect(m_histogram.get(), &HistogramScene::pattern_selected,
            m_subtree_view.get(), &SubtreeView::setNode);

    connect(m_histogram.get(), &HistogramScene::should_be_highlighted,
            this, &SimilarSubtreeWindow::updatePathDiff);

    // m_histogram->reset();
}

/// A wrapper around std::set_intersection; copying is intended
static vector<Label> set_intersect(vector<Label> v1, vector<Label> v2)
{
    /// set_intersection requires the resulting set to be
    /// at least as large as the smallest of the two sets
    vector<Label> res(std::min(v1.size(), v2.size()));

    std::sort(begin(v1), end(v1));
    std::sort(begin(v2), end(v2));

    auto it = std::set_intersection(begin(v1), end(v1), begin(v2), end(v2),
                                    begin(res));
    res.resize(it - begin(res));

    return res;
}

/// A wrapper around std::set_symemtric_diff; copying is intended
static vector<Label> set_symmetric_diff(vector<Label> v1, vector<Label> v2)
{
    vector<Label> res(v1.size() + v2.size());

    /// vectors must be sorted
    std::sort(begin(v1), end(v1));
    std::sort(begin(v2), end(v2));

    /// fill `res` with elements not present in both v1 and v2
    auto it = std::set_symmetric_difference(begin(v1), end(v1), begin(v2), end(v2),
                                            begin(res));

    res.resize(it - begin(res));

    return res;
}

/// Calculate unique labels for both paths (present in one but not in the other)
static std::pair<vector<Label>, vector<Label>>
getLabelDiff(const std::vector<Label> &path1,
             const std::vector<Label> &path2)
{
    auto diff = set_symmetric_diff(path1, path2);

    auto unique_1 = set_intersect(path1, diff);

    auto unique_2 = set_intersect(path2, diff);

    return std::make_pair(std::move(unique_1), std::move(unique_2));
}

/// Walk up the tree constructing label path
static std::vector<Label> labelPath(NodeID nid, const tree::NodeTree &tree)
{
    std::vector<Label> label_path;
    while (nid != NodeID::NoNode)
    {
        auto label = tree.getLabel(nid);
        if (label != emptyLabel)
        {
            label_path.push_back(label);
        }
        nid = tree.getParent(nid);
    }

    return label_path;
}

void SimilarSubtreeWindow::updatePathDiff(const std::vector<NodeID> &nodes)
{
    if (nodes.size() < 2)
        return;

    auto path_l = labelPath(nodes[0], m_nt);
    auto path_r = labelPath(nodes[1], m_nt);

    auto diff_pair = getLabelDiff(path_l, path_r);

    /// unique labels on path_l concatenated
    std::string text_l;

    for (auto &label : diff_pair.first)
    {
        text_l += label + " ";
    }

    std::string text_r;
    for (auto &label : diff_pair.second)
    {
        text_r += label + " ";
    }

    path_line1_.setText(text_l.c_str());
    path_line2_.setText(text_r.c_str());
}

} // namespace analysis
} // namespace cpprofiler
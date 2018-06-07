#include "icicle_canvas.hh"
#include "pixel_image.hh"
#include "pixel_widget.hh"

#include "../utils/tree_utils.hh"
#include "../utils/perf_helper.hh"
#include "../utils/maybe_caller.hh"
#include "../tree/node_tree.hh"

#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QLabel>

namespace cpprofiler
{

namespace pixel_view
{

namespace colors
{
QRgb branch = qRgb(50, 50, 230);
QRgb failure = qRgb(230, 50, 50);
QRgb solution = qRgb(50, 230, 50);
} // namespace colors

class IcicleLayout
{
  public:
    // width for every node
    std::vector<int> width_;
    /// whether node should represent a solution
    std::vector<bool> has_sol;

    void resize(size_t n)
    {
        width_.resize(n);
        has_sol.resize(n);
    }

    inline int width(NodeID n) const { return width_[n]; }
    inline void setWidth(NodeID n, int v) { width_[n] = v; }

    inline bool hasSol(NodeID n) const { return has_sol[n]; }
    inline void setHasSol(NodeID n, bool v) { has_sol[n] = v; }
};

static std::unique_ptr<IcicleLayout> computeLayout(const tree::NodeTree &nt, int compr)
{
    /// TODO: hold tree mutex

    /// post order traversal
    perfHelper.begin("post order");
    auto order = utils::post_order(nt);
    perfHelper.end();

    perfHelper.begin("generations");
    /// All leaf nodes are of generation 1, parent nodes are
    /// the largest generation of children + 1
    std::vector<int> generations(order.size());

    {
        for (auto n : order)
        {
            auto nkids = nt.childrenCount(n);
            if (nkids == 0)
            {
                generations[n] = 1;
            }
            else
            {
                /// assign generation based on children
                int max_gen = 0;
                for (auto alt = 0; alt < nkids; ++alt)
                {
                    const auto kid = nt.getChild(n, alt);
                    if (generations[kid] > max_gen)
                    {
                        max_gen = generations[kid];
                    }
                }
                generations[n] = max_gen + 1;
                // print("node {} is of generation {}", n, generations[n]);
            }
        }
    }
    perfHelper.end();

    perfHelper.begin("icicle layout: rest");
    auto layout = std::unique_ptr<IcicleLayout>(new IcicleLayout());

    layout->resize(order.size());

    for (auto n : order)
    {

        if (generations[n] == compr)
        {
            layout->setWidth(n, 1);

            print("Leaf node: {}", n);

            if (nt.hasSolvedChildren(n))
            {
                print("Set {} as solution", n);
                layout->setHasSol(n, true);
            }
        }
        else if (generations[n] < compr)
        {
            layout->setWidth(n, 0);
        }
        else
        {
            const auto kid1 = nt.getChild(n, 0);

            int sum_width = layout->width(kid1);
            /// extra kids
            auto nkids = nt.childrenCount(n);
            for (auto alt = 1; alt < nkids; ++alt)
            {
                const auto kid = nt.getChild(n, alt);
                sum_width += layout->width(kid);
            }
            layout->setWidth(n, sum_width);
        }
    }
    perfHelper.end();

    return layout;
}

IcicleCanvas::IcicleCanvas(const tree::NodeTree &tree) : QWidget(), tree_(tree)
{

    maybe_caller_.reset(new utils::MaybeCaller(30));
    pimage_.reset(new PixelImage());
    pwidget_.reset(new PixelWidget(*pimage_));

    auto layout = new QVBoxLayout(this);
    layout->addWidget(pwidget_.get());

    connect(pwidget_.get(), &PixelWidget::viewport_resized, [this](const QSize &size) {
        pimage_->resize(size);
        redrawAll();
    });

    auto controlLayout = new QHBoxLayout();
    layout->addLayout(controlLayout);

    {

        controlLayout->addWidget(new QLabel("Zoom:"));

        auto zoomOut = new QPushButton("-", this);
        controlLayout->addWidget(zoomOut);
        connect(zoomOut, &QPushButton::clicked, [this]() {
            pimage_->zoomOut();
            redrawAll();
        });

        auto zoomIn = new QPushButton("+", this);
        controlLayout->addWidget(zoomIn);
        connect(zoomIn, &QPushButton::clicked, [this]() {
            pimage_->zoomIn();
            redrawAll();
        });
    }

    {

        controlLayout->addWidget(new QLabel("Compression:"));

        auto addCompression = new QPushButton("-", this);
        controlLayout->addWidget(addCompression);
        connect(addCompression, &QPushButton::clicked, [this]() {
            compression_ += 1;
            layout_ = computeLayout(tree_, compression_);
            redrawAll();
        });

        auto reduceCompression = new QPushButton("+", this);
        controlLayout->addWidget(reduceCompression);
        connect(reduceCompression, &QPushButton::clicked, [this]() {
            compression_ = std::max(1, compression_ - 1);
            layout_ = computeLayout(tree_, compression_);
            redrawAll();
        });
    }

    // perfHelper.begin("icicle layout");

    layout_ = computeLayout(tree, compression_);

    connect(pwidget_->horizontalScrollBar(), &QScrollBar::valueChanged, [this]() {
        maybe_caller_->call([this]() {
            redrawAll();
        });
    });

    // perfHelper.end();
}

IcicleCanvas::~IcicleCanvas() = default;

void IcicleCanvas::redrawAll()
{

    pimage_->clear();

    drawIcicleTree();

    {
        const auto root = tree_.getRoot();
        const auto total_width = layout_->width_[root];
        /// how many "pixels" fit in one page
        const auto page_width = pwidget_->width();

        /// Note: page width is 1 smaller for the purpose of calculating the srollbar range,
        /// than it is for drawing (this way some "pixels" can be drawn partially at the edge)
        pwidget_->horizontalScrollBar()->setRange(0, total_width - (page_width - 1));
        pwidget_->horizontalScrollBar()->setPageStep(page_width);
    }

    pimage_->update();

    pwidget_->viewport()->update();
}

class IcicleDrawing
{

    const tree::NodeTree &nt_;
    const IcicleLayout &layout_;
    PixelImage &pimage_;
    const int viewport_width;

    int counter = 0;

  private:
    /// Get the color to be used in the icicle tree for this node
    QRgb getColor(NodeID n)
    {
        /// Note: some nodes are not actually solution nodes, but
        /// can represent them when the tree is compressed
        // if (layout_.hasSol(n))
        if (nt_.hasSolvedChildren(n))
        {
            return colors::solution;
        }
        else
        {
            return colors::failure;
        }
        const auto status = nt_.getStatus(n);

        if (status == tree::NodeStatus::BRANCH)
        {
            return colors::branch;
        }
        else if (status == tree::NodeStatus::FAILED)
        {
            return colors::failure;
        }

        return qRgb(255, 255, 255);
    }

    void drawIcicleSubtree(NodeID n, int cur_x, int cur_y)
    {
        const auto width = layout_.width_[n];

        /// no need to draw the node or its children
        if (cur_x > viewport_width || cur_x + width <= 0 || width == 0)
        {
            return;
        }

        /// draw itself
        auto color = getColor(n);

        pimage_.drawRect(cur_x, cur_y, width, color);

        counter++;

        /// draw children
        for (auto alt = 0; alt < nt_.childrenCount(n); ++alt)
        {
            auto kid = nt_.getChild(n, alt);
            drawIcicleSubtree(kid, cur_x, cur_y + 1);
            cur_x += layout_.width_[kid];
        }
    }

  public:
    IcicleDrawing(const tree::NodeTree &nt, IcicleLayout &lo, PixelImage &pi, int viewport_w)
        : nt_(nt), layout_(lo), pimage_(pi), viewport_width(viewport_w)
    {
    }

    ~IcicleDrawing()
    {
        print("drew {} nodes", counter);
    }

    /// only called once
    void run(NodeID root, int x, int y)
    {
        drawIcicleSubtree(root, x, y);
    };
};

void IcicleCanvas::drawIcicleTree()
{

    const auto root = tree_.getRoot();

    const auto x_begin = pwidget_->horizontalScrollBar()->value();

    perfHelper.begin("draw icicle");

    IcicleDrawing drawer(tree_, *layout_, *pimage_, pwidget_->width());
    drawer.run(root, -x_begin, 0);

    perfHelper.end();

    /// provided every node knows its bounding box
}

} // namespace pixel_view
} // namespace cpprofiler
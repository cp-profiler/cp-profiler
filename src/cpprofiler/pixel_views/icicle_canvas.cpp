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

/// left and right bound in 'pixels'
struct Bounds
{
    int width;
};

struct IcicleLayout
{
    // bounds for every node
    std::vector<Bounds> bounds_;
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

    auto &bounds = layout->bounds_;
    bounds.resize(order.size());

    for (auto n : order)
    {

        if (generations[n] == compr)
        {
            bounds[n].width = 1;
        }
        else if (generations[n] < compr)
        {
            bounds[n].width = 0;
        }
        else
        {
            const auto kid1 = nt.getChild(n, 0);
            bounds[n].width = bounds[kid1].width;
            /// extra kids
            auto nkids = nt.childrenCount(n);
            for (auto alt = 1; alt < nkids; ++alt)
            {
                const auto kid = nt.getChild(n, alt);
                bounds[n].width += bounds[kid].width;
            }
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
        const auto total_width = layout_->bounds_[root].width;
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
    void drawIcicleSubtree(NodeID n, int cur_x, int cur_y)
    {
        const auto width = layout_.bounds_[n].width;

        /// no need to draw the node or its children
        if (cur_x > viewport_width || cur_x + width <= 0 || width == 0)
        {
            return;
        }

        /// draw itself
        QRgb color = qRgb(30, 40, 30);
        pimage_.drawRect(cur_x, cur_y, width, color);

        counter++;

        /// draw children
        for (auto alt = 0; alt < nt_.childrenCount(n); ++alt)
        {
            auto kid = nt_.getChild(n, alt);
            drawIcicleSubtree(kid, cur_x, cur_y + 1);
            cur_x += layout_.bounds_[kid].width;
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
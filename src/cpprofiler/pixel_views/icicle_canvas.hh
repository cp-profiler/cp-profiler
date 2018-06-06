#pragma once

#include <QWidget>
#include <memory>

namespace cpprofiler
{

namespace utils
{
class MaybeCaller;
}

namespace tree
{
class NodeTree;
}

namespace pixel_view
{

class PixelImage;
class PixelWidget;
class IcicleLayout;

class IcicleCanvas : public QWidget
{
    Q_OBJECT
    const tree::NodeTree &tree_;

    std::unique_ptr<PixelImage> pimage_;
    std::unique_ptr<PixelWidget> pwidget_;

    std::unique_ptr<IcicleLayout> layout_;

    std::unique_ptr<utils::MaybeCaller> maybe_caller_;

    /// Determines how many times leaf nodes have been chopped off
    int compression_ = 2;

  public:
    IcicleCanvas(const tree::NodeTree &tree);

    ~IcicleCanvas();

    void redrawAll();

    void drawIcicleTree();
};
} // namespace pixel_view
} // namespace cpprofiler
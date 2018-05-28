#include "canvas.hh"
#include "pixel_image.hh"

#include <QPainter>

namespace cpprofiler
{

namespace pixel_tree
{

Canvas::Canvas() : QAbstractScrollArea()
{
    pimage_.reset(new PixelImage());
}

void Canvas::paintEvent(QPaintEvent *e)
{

    QPainter painter{viewport()};

    painter.drawImage(QPoint(0, 0), pimage_->raw_image());
}

Canvas::~Canvas() = default;

} // namespace pixel_tree
} // namespace cpprofiler
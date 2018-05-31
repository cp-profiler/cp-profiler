#include "pixel_widget.hh"

#include <QScrollBar>

namespace cpprofiler
{

namespace pixel_tree
{

void PixelWidget::mousePressEvent(QMouseEvent *e)
{
    const auto xoff = horizontalScrollBar()->value();

    const int vline = e->x() / image_.pixel_size() + xoff;

    pressed_vline_ = vline;
}

void PixelWidget::mouseReleaseEvent(QMouseEvent *e)
{
    const auto xoff = horizontalScrollBar()->value();

    const int vline = e->x() / image_.pixel_size() + xoff;

    if (pressed_vline_ <= vline)
        emit slices_selected(pressed_vline_, vline);
    else
        emit slices_selected(vline, pressed_vline_);
}

} // namespace pixel_tree
} // namespace cpprofiler
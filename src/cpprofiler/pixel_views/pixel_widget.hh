#pragma once

#include <QAbstractScrollArea>
#include <QPainter>
#include <QResizeEvent>

#include <cmath> // std::ceil

#include "pixel_image.hh"
#include "../utils/debug.hh"

namespace cpprofiler
{

namespace pixel_view
{

class PixelWidget : public QAbstractScrollArea
{
    Q_OBJECT
    const PixelImage &image_;

    /// record previosly clicked vline
    int pressed_vline_ = -1;

  protected:
    void paintEvent(QPaintEvent *e) override
    {
        QPainter painter{viewport()};
        painter.drawImage(QPoint(5, 5), image_.raw_image());
    }

    void resizeEvent(QResizeEvent *e) override
    {
        viewport_resized(e->size());
    }

    void mousePressEvent(QMouseEvent *e) override;

    void mouseReleaseEvent(QMouseEvent *e) override;

  public:
    PixelWidget(const PixelImage &image) : image_(image) {}

    /// How many "pixels" fit in one page;
    int width() const
    {

        return std::ceil((float)(viewport()->width() - 10) / image_.pixel_size());
    }

  signals:
    void viewport_resized(const QSize &size);

    /// notify pixel canvas that some slices have been selected
    /// (if only one slice selected, begin = end);
    /// indexes may refer to non-existing slices
    void slices_selected(int begin, int end);
};

} // namespace pixel_view

} // namespace cpprofiler
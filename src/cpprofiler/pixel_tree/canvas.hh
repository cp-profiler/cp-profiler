#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QAbstractScrollArea>
#include <memory>

namespace cpprofiler
{

namespace pixel_tree
{

class PixelImage;

class Canvas : public QAbstractScrollArea
{

    std::unique_ptr<PixelImage> pimage_;

  protected:
    void paintEvent(QPaintEvent *e) override;

  public:
    Canvas();

    ~Canvas();
};

} // namespace pixel_tree
} // namespace cpprofiler
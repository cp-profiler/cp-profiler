#pragma once

#include <cstdint>
#include <vector>
#include <memory>

class QImage;

namespace cpprofiler
{

namespace pixel_tree
{
class PixelImage
{
  private:
    /// Buffer used to initialize QImage
    std::vector<uint32_t> buffer_;

    /// The image used for diplaying the tree
    std::unique_ptr<QImage> image_;

    int width_ = 40;
    int height_ = 20;

  public:
    PixelImage();

    /// change QImage to match the buffer
    void update();

    void resize(int width, int height);

    void clear();

    const QImage &raw_image() { return *image_; }
};
} // namespace pixel_tree
} // namespace cpprofiler
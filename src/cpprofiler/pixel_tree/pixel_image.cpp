#include "pixel_image.hh"

#include <QImage>

namespace cpprofiler
{

namespace pixel_tree
{

PixelImage::PixelImage()
{
    image_.reset(new QImage());

    resize(width_, height_);
}

void PixelImage::resize(int width, int height)
{
    buffer_.clear();

    buffer_.resize(width * height);

    width_ = width;
    height_ = height;

    clear();
    update();
}

void PixelImage::clear()
{
    /// set all pixels to white
    // std::fill(buffer_.begin(), buffer_.end(), 0xFFFFFF);
    std::fill(buffer_.begin(), buffer_.end(), 0x0000FF);
}

void PixelImage::update()
{

    auto buf = reinterpret_cast<uint8_t *>(buffer_.data());
    image_.reset(new QImage(buf, width_, height_, QImage::Format_RGB32));
}

} // namespace pixel_tree
} // namespace cpprofiler
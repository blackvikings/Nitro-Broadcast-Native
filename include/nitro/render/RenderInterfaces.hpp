#pragma once

#include <QImage>
#include <QString>
#include <cstdint>
#include <memory>
#include <vector>

namespace nitro {

enum class PixelFormat {
    Unknown,
    BGRA8,
    RGBA8,
    R8
};

struct VideoFrameInfo {
    int width = 0;
    int height = 0;
    PixelFormat format = PixelFormat::Unknown;
    std::uint64_t ptsNs = 0;
    QString sourceId;
};

class IVideoFrame {
public:
    virtual ~IVideoFrame() = default;
    virtual VideoFrameInfo info() const = 0;
    virtual const std::uint8_t* data() const = 0;
    virtual std::size_t dataSize() const = 0;
    virtual int strideBytes() const = 0;
};

/// CPU-backed frame for scaffolding (WGC/GPU textures come later).
class CpuVideoFrame final : public IVideoFrame {
public:
    CpuVideoFrame() = default;
    explicit CpuVideoFrame(VideoFrameInfo info, std::vector<std::uint8_t> pixels, int stride)
        : info_(info)
        , pixels_(std::move(pixels))
        , stride_(stride)
    {
    }

    VideoFrameInfo info() const override { return info_; }
    const std::uint8_t* data() const override { return pixels_.data(); }
    std::size_t dataSize() const override { return pixels_.size(); }
    int strideBytes() const override { return stride_; }

    QImage toQImage() const
    {
        if (info_.width <= 0 || info_.height <= 0 || pixels_.empty()) {
            return {};
        }
        QImage::Format fmt = QImage::Format_ARGB32;
        if (info_.format == PixelFormat::RGBA8) {
            fmt = QImage::Format_RGBA8888;
        }
        return QImage(pixels_.data(), info_.width, info_.height, stride_, fmt).copy();
    }

private:
    VideoFrameInfo info_;
    std::vector<std::uint8_t> pixels_;
    int stride_ = 0;
};

class ICompositor {
public:
    virtual ~ICompositor() = default;
    virtual void setCanvasSize(int width, int height) = 0;
    virtual int canvasWidth() const = 0;
    virtual int canvasHeight() const = 0;
    /// Compose current scene into an output frame (placeholder returns solid canvas).
    virtual std::shared_ptr<IVideoFrame> compose() = 0;
};

class PreviewRenderer {
public:
    void setCompositor(ICompositor* c) { compositor_ = c; }
    std::shared_ptr<IVideoFrame> render()
    {
        return compositor_ ? compositor_->compose() : nullptr;
    }

private:
    ICompositor* compositor_ = nullptr;
};

class ProgramRenderer {
public:
    void setCompositor(ICompositor* c) { compositor_ = c; }
    std::shared_ptr<IVideoFrame> render()
    {
        return compositor_ ? compositor_->compose() : nullptr;
    }

private:
    ICompositor* compositor_ = nullptr;
};

/// Placeholder compositor — black/solid frame until capture pipeline exists.
class NullCompositor final : public ICompositor {
public:
    void setCanvasSize(int width, int height) override
    {
        width_ = width;
        height_ = height;
    }
    int canvasWidth() const override { return width_; }
    int canvasHeight() const override { return height_; }

    std::shared_ptr<IVideoFrame> compose() override
    {
        VideoFrameInfo info;
        info.width = width_;
        info.height = height_;
        info.format = PixelFormat::BGRA8;
        const int stride = width_ * 4;
        std::vector<std::uint8_t> px(static_cast<std::size_t>(stride) * height_, 0);
        // Dark studio background BGRA
        for (int y = 0; y < height_; ++y) {
            for (int x = 0; x < width_; ++x) {
                auto* p = px.data() + static_cast<std::size_t>(y * stride + x * 4);
                p[0] = 0x14; // B
                p[1] = 0x16; // G
                p[2] = 0x1C; // R
                p[3] = 0xFF;
            }
        }
        return std::make_shared<CpuVideoFrame>(info, std::move(px), stride);
    }

private:
    int width_ = 1920;
    int height_ = 1080;
};

} // namespace nitro

#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

// 在Windows下定义导出方式为DLL，在其他平台上不需要EXPORT关键字
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <opencv2/opencv.hpp>

class ImageProcessor {
public:
    // @param width: 11
    ImageProcessor(unsigned char *bitmapData, int width, int height, int stride);

    // 图像处理操作，返回自身以支持链式调用
    ImageProcessor &Translate(double offsetX, double offsetY);

    ImageProcessor &Rotate(double angle);

    ImageProcessor &Overlay(const cv::Mat &baseImage, const cv::Mat &overlayImage, int x, int y);

    ImageProcessor &SmoothEdges(int radius);

    ImageProcessor &Scale(double scaleX, double scaleY);

    ImageProcessor &SetOpacity(double opacity);

    ImageProcessor &Flip(bool horizontal, bool vertical);

    ImageProcessor &Crop(int startX, int startY, int width, int height);

    ImageProcessor &AdjustBrightnessContrast(double brightness, double contrast);

    ImageProcessor &SetRotationCenter(int centerX, int centerY);

    ImageProcessor &PerspectiveTransform(const std::vector<cv::Point> &srcPoints,
                                         const std::vector<cv::Point> &dstPoints);

    ImageProcessor &AdjustColor(int red, int green, int blue);

    // 获取处理后的结果
    cv::Mat GetResult() const;

private:
    cv::Mat image;
};

#endif //IMAGEPROCESSOR_H

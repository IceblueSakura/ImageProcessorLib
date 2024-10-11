#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

// 在Windows下定义导出方式为DLL，在其他平台上不需要EXPORT关键字
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <opencv2/core/cuda.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaimgproc.hpp>

class ImageProcessor {
private:
    cv::Mat hostImage;
    cv::cuda::GpuMat deviceImage;
    bool useGPU;

    enum DeviceType {
        CPU = 0,
        GPU = 1
    };

    DeviceType lastProcessed = DeviceType::CPU; // 0 = CPU,1=GPU,or impl enum marked task name.

    // 判断GPU是否可用
    static bool isGPUAvailable();

    // 上载数据到GPU，并设置最后处理为GPU
    void uploadToGPU();

    // 下载数据回RAM，并设置最后处理为CPU
    void downloadFromGPU();

    // 从24bbp RGB图像加载数据，即MatType.CV_8UC3(uchar, 3 channel)
    // 如果需要扩展图像类型就模仿这个方法再写一个对应的，并在构造函数补充
    void format24bppRgb(unsigned char *bitmapData, int width, int height);

public:
    // stride(step): 行步幅，在这意味着每row的内存偏移。因为内存对齐原因，所以不等于width*data-size
    ImageProcessor(unsigned char *bitmapData,
                   int width, int height,
                   unsigned char channel = 3,  // RGB
                   bool useGPU = true);

    // 调整亮度与对比度
    ImageProcessor &AdjustBrightnessContrast(int brightness, double contrast);

    // 设置透明度
    ImageProcessor &SetOpacity(double opacity);

    // 水平horizontal/垂直vertical翻转图像
    ImageProcessor &Flip(bool horizontal, bool vertical);

    // 透视变换
    ImageProcessor &PerspectiveTransform(const std::vector<cv::Point> &srcPoints,
                                         const std::vector<cv::Point> &dstPoints);

    // 调整特定颜色的颜色，调整后的值为0~255(24bppRGB, 8bit)
    ImageProcessor &AdjustColor(int red, int green, int blue);

    // 按比例缩放
    ImageProcessor &Scale(double scaleX, double scaleY);

    // 切割矩形
    ImageProcessor &CropRectangle(int startX, int startY, int width, int height);

    // 设置旋转中心，未设置时为几何中心
    ImageProcessor &SetRotationCenter(int centerX, int centerY);

    // 旋转特定角度
    ImageProcessor &Rotate(double angle);

    // 将overlayImage覆盖到baseImage的特定区域座标处
    ImageProcessor &Overlay(cv::Mat &baseImage, cv::Mat &overlayImage, int x, int y);

    // 图像平移，空白处使用0填充
    ImageProcessor &Translate(double offsetX, double offsetY);

    // 以特定像素值平滑边缘，用于切割操作后
    ImageProcessor &SmoothEdges(int radius);

    // 将内存中的图像复制到GPU中
    ImageProcessor &CopyToGPU();

    // 将GPU中的图像复制到内存中
    ImageProcessor &CopyToHost();


    // 获取当前结果
    cv::Mat GetMat();
};

#endif //IMAGEPROCESSOR_H

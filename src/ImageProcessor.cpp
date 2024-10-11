#include <iostream>
#include "ImageProcessor.h"

using namespace std;

bool ImageProcessor::isGPUAvailable() {
    if (cv::cuda::getCudaEnabledDeviceCount() > 0) {
        return true;
    }
    return false;
}

void ImageProcessor::uploadToGPU() {
    if (this->lastProcessed != DeviceType::GPU) {
        this->deviceImage.upload(this->hostImage);
        this->lastProcessed = DeviceType::GPU;
    }
}

void ImageProcessor::downloadFromGPU() {
    if (this->lastProcessed != DeviceType::CPU) {
        this->deviceImage.download(this->hostImage);
        this->lastProcessed = DeviceType::CPU;
    }
}

// bitmap的stride应该在csharp端进行转换，所以在此默认内存已经是紧密的
void ImageProcessor::format24bppRgb(unsigned char *bitmapData, const int width, const int height) {
    this->hostImage = cv::Mat(width, height, CV_8UC3, bitmapData).clone(); // 深拷贝内存，之后不阻塞调用方取回bitmap控制权
    this->lastProcessed = DeviceType::CPU; // 最后处理的设备是host
}


ImageProcessor::ImageProcessor(unsigned char *bitmapData,
                               const int width, const int height,
                               const unsigned char channel,
                               bool useGPU) {
    // 初始化图像处理的具体逻辑
    switch (channel) {
        case 1:
        case 2:
        case 3:
            format24bppRgb(bitmapData, width, height);
            break;
        default:
            throw invalid_argument("错误的图像通道数！");
    }
    // 判断是否需要使用GPU，及GPU是否可用
    if (useGPU && isGPUAvailable()) {
        this->useGPU = true;
        uploadToGPU();
    }
    // 构造函数本身就会返回它自身，无需return
}

ImageProcessor &ImageProcessor::Translate(const double offsetX, const double offsetY) {
    // 实现平移操作
    const cv::Mat transformationMatrix = (cv::Mat_<double>(2, 3) << 1, 0, offsetX, 0, 1, offsetY); // 构造操作矩阵
    if (useGPU) {
        cv::cuda::GpuMat result;
        cv::cuda::warpAffine(deviceImage, result, transformationMatrix, deviceImage.size());
        deviceImage = result; // 不使用同一矩阵变量作为输入及输出，因为这样可能会造成异常
    } else {
        cv::warpAffine(hostImage, hostImage, transformationMatrix, hostImage.size());
    }
    return *this; // 返回自身支持链式调用
}

// 调整亮度与对比度
ImageProcessor &ImageProcessor::AdjustBrightnessContrast(const int brightness, const double contrast) {
    if (useGPU) {
        const cv::cuda::GpuMat result;

        deviceImage = result;
    } else {
        hostImage.convertTo(hostImage, -1, contrast, brightness);
    }
    return *this;
}

// 获取当前结果
cv::Mat ImageProcessor::GetMat() {
    if (lastProcessed == DeviceType::GPU) {
        downloadFromGPU(); // 下载回RAM
    }
    return hostImage;
}

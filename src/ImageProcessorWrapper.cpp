#include "ImageProcessor.h"

// 使用extern "C"避免C++的名称修饰，使函数以C风格导出
extern "C" {
// 创建ImageProcessor对象
EXPORT ImageProcessor *CreateImageProcessor(unsigned char *bitmapData,
                                            const int width, const int height, const unsigned char channel = 3,
                                            const bool useGPU) {
    try {
        return new ImageProcessor(bitmapData, width, height, channel, useGPU);
    } catch (...) {
        return nullptr;
    }
}

// 释放ImageProcessor对象
EXPORT void DestroyImageProcessor(const ImageProcessor *processor) {
    delete processor;
}

EXPORT int AdjustBrightnessContrast(ImageProcessor *processor, int brightness, double contrast) {
    if (processor == nullptr) return -1;
    try {
        processor->AdjustBrightnessContrast(brightness, contrast);
        return 0;
    } catch (...) {
        return -1;
    }
}
    // 一坨处理

EXPORT int GetProcessedImage(ImageProcessor *processor, unsigned char **data, int *width, int *height, int *channels) {
    if (processor == nullptr || data == nullptr || width == nullptr || height == nullptr || channels == nullptr) {
        return -1;
    }
    try {
        const cv::Mat img = processor->GetMat();
        *width = img.cols;
        *height = img.rows;
        *channels = img.channels();
        const size_t dataSize = img.total() * img.elemSize();
        auto *outputData = new unsigned char[dataSize];
        memcpy(outputData, img.data, dataSize);
        *data = outputData;
        return 0;
    } catch (...) {
        return -1;
    }
}


}

#include "ImageProcessorWrapper.h"
#include "ImageProcessor.h"

// 使用extern "C"避免C++的名称修饰，使函数以C风格导出
extern "C" {
// 创建ImageProcessor对象
EXPORT ImageProcessor *CreateImageProcessor(unsigned char *bitmapData,
                                            const int width, const int height, const unsigned char channel,
                                            const bool useGPU) {
    try {
        return new ImageProcessor(bitmapData, width, height, channel, useGPU);
    } catch (...) {
        return nullptr;
    }
}

// 释放ImageProcessor对象
EXPORT void DestroyImageProcessor(ImageProcessor *processor) {
    delete processor;
}

// 获取处理后的图像，放入data二重指针中(节省调用栈)
EXPORT int GetProcessedImage(ImageProcessor *processor, unsigned char **data, int *width, int *height, int *channels) {
    if (processor == nullptr || data == nullptr || width == nullptr || height == nullptr || channels == nullptr) {
        return -1; // 返回错误代码
    }
    try {
        ImageProcessor::GetMatData(processor->GetMat(), data, width, height, channels);
        return 0; // 成功
    } catch (...) {
        return -1; // 处理异常
    }
}

// 缩放图像
EXPORT int Scale(ImageProcessor *processor, double scaleX, double scaleY) {
    if (processor == nullptr) {
        return -1; // 返回错误代码
    }
    try {
        processor->Scale(scaleX, scaleY);
        return 0; // 成功
    } catch (...) {
        return -1; // 处理异常
    }
}


// 裁剪矩形区域
EXPORT int CropRectangle(ImageProcessor *processor, int startX, int startY, int width, int height) {
    if (processor == nullptr) {
        return -1; // 返回错误代码
    }
    try {
        processor->CropRectangle(startX, startY, width, height);
        return 0; // 成功
    } catch (...) {
        return -1; // 处理异常
    }
}

// 设置旋转中心
EXPORT int SetRotationCenter(ImageProcessor *processor, float centerX, float centerY) {
    if (processor == nullptr) {
        return -1; // 返回错误代码
    }
    try {
        processor->SetRotationCenter(centerX, centerY);
        return 0; // 成功
    } catch (...) {
        return -1; // 处理异常
    }
}

// 旋转图像
EXPORT int Rotate(ImageProcessor *processor, double angle) {
    if (processor == nullptr) {
        return -1; // 返回错误代码
    }
    try {
        processor->Rotate(angle);
        return 0; // 成功
    } catch (...) {
        return -1; // 处理异常
    }
}


// 平移图像
EXPORT int Translate(ImageProcessor *processor, double offsetX, double offsetY) {
    if (processor == nullptr) {
        return -1; // 返回错误代码
    }
    try {
        processor->Translate(offsetX, offsetY);
        return 0; // 成功
    } catch (...) {
        return -1; // 处理异常
    }
}

// 合并两张图，这个操作还是留给csharp把，不然指针传递太痛苦了
// EXPORT int Overlay(unsigned char **baseImage, unsigned char *overlayImage, int x, int y) {
//     return 0;
// }
}

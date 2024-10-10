#include "ImageProcessor.h"

// 使用extern "C"避免C++的名称修饰，使函数以C风格导出
extern "C" {
// 创建ImageProcessor对象
EXPORT ImageProcessor *CreateImageProcessor(unsigned char *bitmapData, int width, int height, int stride) {
    return new ImageProcessor();
}

// 释放ImageProcessor对象
EXPORT void DestroyImageProcessor(ImageProcessor *processor) {
    delete processor;
}

// 图像处理函数，返回处理器本身的指针以支持链式调用
EXPORT ImageProcessor *Translate(ImageProcessor *processor, double offsetX, double offsetY) {
    return &processor->Translate(offsetX, offsetY); // 返回对象指针
}

EXPORT ImageProcessor *Rotate(ImageProcessor *processor, double angle) {
    return &processor->Rotate(angle); // 返回对象指针
}

// 导出其他图像处理操作...
}

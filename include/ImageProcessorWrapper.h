#ifndef IMAGEPROCESSORWRAPPER_H
#define IMAGEPROCESSORWRAPPER_H

#ifdef ImageProcessorLib_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif


#include "ImageProcessor.h"

// 使用extern "C"避免C++的名称修饰，使函数以C风格导出
extern "C" {
    // 创建ImageProcessor对象
    EXPORT ImageProcessor *CreateImageProcessor(unsigned char *bitmapData,
                                                const int width, const int height, const unsigned char channel,
                                                const bool useGPU);

    // 释放ImageProcessor对象
    EXPORT void DestroyImageProcessor(ImageProcessor *processor);

    // 获取处理后的图像，放入data二重指针中(节省调用栈)
    EXPORT int GetProcessedImage(ImageProcessor *processor, unsigned char **data, int *width, int *height, int *channels);

    // 缩放图像
    EXPORT int Scale(ImageProcessor *processor, double scaleX, double scaleY);


    // 裁剪矩形区域
    EXPORT int CropRectangle(ImageProcessor *processor, int startX, int startY, int width, int height);

    // 设置旋转中心
    EXPORT int SetRotationCenter(ImageProcessor *processor, float centerX, float centerY);

    // 旋转图像
    EXPORT int Rotate(ImageProcessor *processor, double angle);

    // 平移图像
    EXPORT int Translate(ImageProcessor *processor, double offsetX, double offsetY);

// 合并两张图，这个操作还是留给csharp把，不然指针传递太痛苦了
// EXPORT int Overlay(unsigned char **baseImage, unsigned char *overlayImage, int x, int y) {
//     return 0;
// }
}

#endif //IMAGEPROCESSORWRAPPER_H

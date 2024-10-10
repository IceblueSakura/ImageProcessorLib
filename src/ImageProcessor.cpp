#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include "ImageProcessor.h"

using namespace std;

ImageProcessor::ImageProcessor(unsigned char *bitmapData, int width, int height, int stride) {
    // 初始化图像处理的具体逻辑
    image = cv::imdecode(bitmapData,cv::IMREAD_COLOR);
    cout << "ImageProcessor::ImageProcessor()" << endl;
}

ImageProcessor &ImageProcessor::Translate(double offsetX, double offsetY) {
    // 实现平移操作
    return *this; // 返回自身支持链式调用
}

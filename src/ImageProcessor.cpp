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
void ImageProcessor::format24bppRgb(unsigned char *bitmapData, const int width, const int height, const int channel) {
    this->hostImage = cv::Mat(width, height, CV_8UC(channel), bitmapData).clone();
    // 深拷贝内存，之后不阻塞调用方取回bitmap控制权。这也是为了分辨率变换考虑
    this->lastProcessed = DeviceType::CPU; // 最后处理的设备是host
}

ImageProcessor::ImageProcessor(unsigned char *bitmapData,
                               const int width, const int height,
                               const unsigned char channel,
                               const bool useGPU) {
    // 初始化图像处理的具体逻辑
    switch (channel) {
        case 1:
        case 2:
        case 3:
            format24bppRgb(bitmapData, width, height, channel);
            break;
        default:
            throw invalid_argument("错误的图像通道数！目前还不支持32bbpARGB图像");
    }
    // 判断是否需要使用GPU，及GPU是否可用
    if (useGPU && isGPUAvailable()) {
        this->useGPU = true;
        uploadToGPU();
    }
    // 构造函数本身就会返回它自身，无需return
}

ImageProcessor &ImageProcessor::Scale(const double scaleX, const double scaleY) {
    // 实现缩放操作
    if (useGPU) {
        uploadToGPU();
        cv::cuda::GpuMat result;
        cv::cuda::resize(deviceImage, result, cv::Size(), scaleX, scaleY);
        deviceImage = result;
    } else {
        downloadFromGPU();
        cv::resize(hostImage, hostImage, cv::Size(), scaleX, scaleY);
    }
    return *this;
}

ImageProcessor &ImageProcessor::CropRectangle(const int startX, const int startY, const int width, const int height) {
    // 实现裁剪矩形操作
    if (useGPU) {
        uploadToGPU();
        const cv::Rect roi(startX, startY, width, height);
        deviceImage = deviceImage(roi);
    } else {
        downloadFromGPU();
        cv::Rect roi(startX, startY, width, height);
        hostImage = hostImage(roi);
    }
    return *this;
}

ImageProcessor &ImageProcessor::SetRotationCenter(const float centerX, const float centerY) {
    // 设置旋转中心点位置
    this->rotationCenter = cv::Point2f(centerX, centerY);
    return *this;
}

ImageProcessor &ImageProcessor::Rotate(const double angle) {
    // 实现旋转操作，以rotationCenter为中心
    const auto transformationMatrix = cv::getRotationMatrix2D(rotationCenter, angle, 1.0); // 构造旋转矩阵
    if (useGPU) {
        uploadToGPU();
        cv::cuda::GpuMat result;
        cv::cuda::warpAffine(deviceImage, result, transformationMatrix, deviceImage.size());
        deviceImage = result;
    } else {
        downloadFromGPU();
        cv::warpAffine(hostImage, hostImage, transformationMatrix, hostImage.size());
    }
    return *this;
}

ImageProcessor &ImageProcessor::Translate(const double offsetX, const double offsetY) {
    // 实现平移操作
    const cv::Mat transformationMatrix = (cv::Mat_<double>(2, 3) << 1, 0, offsetX, 0, 1, offsetY); // 构造操作矩阵
    if (useGPU) {
        uploadToGPU();
        cv::cuda::GpuMat result;
        cv::cuda::warpAffine(deviceImage, result, transformationMatrix, deviceImage.size());
        deviceImage = result; // 不使用同一矩阵变量作为输入及输出，因为这样可能会造成异常
    } else {
        downloadFromGPU();
        cv::warpAffine(hostImage, hostImage, transformationMatrix, hostImage.size());
    }
    return *this; // 返回自身支持链式调用
}


cv::Mat &ImageProcessor::GetMat() {
    if (useGPU) {
        downloadFromGPU(); // 下载回RAM
    }
    return hostImage; // 返回的是引用传递，等效于指针
}

void ImageProcessor::GetMatData(const cv::Mat &mat, unsigned char **data, int *width, int *height, int *channels) {
    // 先指向Mat的指针，如果出现指针无数据时可能需要考虑是否需要从mat中先复制出数据，再初始化数组
    *width = mat.cols;
    *height = mat.rows;
    *channels = mat.channels(); // e.g. CV_8CU3,channel = 3
    const size_t dataSize = mat.total() * mat.elemSize();
    // 总像素个数*每个像素体积(如8bit-RGB，每像素三个8bit(因为三通道)，那么这个值就是3) = 要分配的元素空间
    auto *outputData = new unsigned char[dataSize]; // 新分配一块内存，和矩阵空间相同
    // 直接把数据具体内容复制到output
    memcpy(outputData, mat.data, dataSize); // 从mat pointer把数据复制到新内存区域
    *data = outputData;
}


void ImageProcessor::Overlay(cv::Mat &baseImage, const cv::Mat &overlayImage, int x, int y) {
    if (x + overlayImage.cols > baseImage.cols || y + overlayImage.rows > baseImage.rows) {
        throw invalid_argument("要覆盖的图像超出原图边界！请先尝试调整覆盖图像尺寸或覆盖位置。");
    }
    auto baseRoi = cv::Rect(x, y, overlayImage.cols, overlayImage.rows);
    overlayImage.copyTo(baseImage(baseRoi));
}

void ImageProcessor::Overlay(cv::cuda::GpuMat &baseImage, const cv::cuda::GpuMat &overlayImage, int x, int y) {
    if (x + overlayImage.cols > baseImage.cols || y + overlayImage.rows > baseImage.rows) {
        throw invalid_argument("要覆盖的图像超出原图边界！请先尝试调整覆盖图像尺寸或覆盖位置。");
    }
    auto baseRoi = baseImage(cv::Rect(x, y, overlayImage.cols, overlayImage.rows)); // 从背景扣一块候选区域
    overlayImage.copyTo(baseRoi); // 把前景复制到候选区域
}

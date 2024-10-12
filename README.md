# ImageProcessor Cpp
一个C++编写的opencv支持库，主要测试的调用方式是P/Invoke(DLL)

## 使用需求
- cmake 3.10+
- 

## 流程设计
### csharp
1. [C#] 创建 `ImageProcessorWrapper` 类，并绑定 `Bitmap` 对象，在这之后cpp不会阻塞线程。
   - 初始化 `ImageProcessorWrapper` 实例及`ImageProcessor`实例，并复制图像资源，准备进行图像处理。
2. [C#] 链式调用 `ImageProcessorWrapper` 类中的方法，设计变换流程，cpp会同步处理它复制的那份内存数据。
   - 通过链式调用的方式设置图像处理的操作序列。
3. [C#] 调用 `ImageProcessorWrapper::GetProcessedImage`将获取处理后的数据，这将创建一个新的 `Bitmap`，并将新的的 `Bitmap`返回。
   - 因为图像分辨率和通道数可能改变，所以不在原本内存位置进行处理。

### cpp
1. [C++] 在初始化后，将指针区域的长度等用于校验的数据保存（防止内存越界），并将 RAM 复制到 VRAM
   - 在 C++ 端初始化时，保存指针区域的信息以防止内存越界，将图像数据从 RAM 复制到 VRAM 中进行处理。
2. [C++] 之后的变换全部都在 VRAM/RAM 中统一运行，减少复制导致的延迟，直到结束所有序列操作后，再将数据复制回 RAM
   - 所有图像处理操作都在 VRAM 中完成，处理结束后将结果数据从 VRAM 复制回 RAM。
3. [C++] 释放 VRAM，但不要释放 RAM（因为内存控制权即将转回C#）
   - 释放 VRAM 资源，但保留 RAM 的控制权，以便 C# 端继续操作。
4. [C++] 获取图像时以二重指针的形式返回新的图像数据

处理流程结束。

## 需要的功能
1. 图像XYWH座标的矩形切分
2. 色调改变，如灰度
3. 图像旋转
4. 边缘平滑
5. 加载第二张图像，并移动第一张图像覆盖到第二章张图像的特定位置

## 类结构设计
- [csharp]因为需要以队列形式记录操作序列最终统一提交，所以设计链式调用的`ImageProcessorWrapper`类。
- [cpp]为了需要减少RAM/VRAM之间的数据复制，故尽可能所有操作都在一次复制后全部在VRAM中执行后再传递回RAM。为此，可能也需要链式调用。

## P/Invoke接口定义
### Csharp
- Create()
-

### Cpp
- Create()
-


## Cpp操作类接口定义
为了在Csharp中定义类似的功能结构，减少思维负担，所以尽可能在csharp中定义与cpp中结构类似的接口。
- Create()
-

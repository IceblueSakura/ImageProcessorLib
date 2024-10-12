# ImageProcessor Cpp
一个使用OpenCV编写的C++图像处理库，支持通过P/Invoke在C#中调用。

## 使用需求
- CMake 3.10+
- OpenCV 4.x
- 支持CUDA的显卡（用于GPU加速）

## 流程设计
### CSharp
1. **初始化**：
   - 创建 `ImageProcessorWrapper` 类实例，绑定 `Bitmap` 对象。在创建过程中，`ImageProcessor`实例也会被初始化，图像数据会被复制以供处理。

2. **链式调用**：
   - 使用链式调用设计变换流程，调用`ImageProcessorWrapper`中的方法来设置不同的图像处理操作，这些操作在CPP端同步执行。

3. **获取结果**：
   - 调用`ImageProcessorWrapper::GetProcessedImage`来获取处理后的数据。该方法会返回一个新的`Bitmap`对象，因为图像的分辨率和通道数可能会在处理过程中发生变化。

### Cpp
1. **初始化**：
   - 在初始化过程中，保存传入图像数据的指针信息以防止内存越界，并将图像数据从RAM复制到VRAM（如果启用GPU）。

2. **图像处理**：
   - 所有图像变换操作在VRAM或RAM中完成，以减少数据复制所带来的延迟。操作结束后，将最终处理结果从VRAM复制回RAM。

3. **资源管理**：
   - 释放VRAM资源，但保留RAM中的数据，以便将控制权交回C#。

4. **数据返回**：
   - 通过二重指针返回处理后的图像数据，这样可以在C#中方便地创建新的`Bitmap`。

## 需要的功能
1. 矩形裁剪：根据指定的XYWH坐标进行图像裁剪。
2. 图像缩放：按比例缩放图像。
3. 图像旋转：围绕指定中心点旋转图像。
4. 图像平移：将图像移动指定的偏移量。
5. 图像叠加：加载第二张图像，并将第一张图像覆盖到第二张图像的特定位置。

## 类结构设计
- **CSharp**：设计`ImageProcessorWrapper`类，以链式调用方式调用DLL。
- **Cpp**：尽量减少RAM和VRAM之间的数据传输。

## P/Invoke接口定义
### CSharp
- **CreateImageProcessor**：用于创建`ImageProcessor`实例。
- **DestroyImageProcessor**：释放`ImageProcessor`实例。
- **GetProcessedImage**：获取处理后的图像数据。
- **Scale**：缩放图像。
- **CropRectangle**：裁剪图像。
- **SetRotationCenter**：设置旋转中心。
- **Rotate**：旋转图像。
- **Translate**：平移图像。

### Cpp
- **CreateImageProcessor**：创建并初始化`ImageProcessor`对象。
- **DestroyImageProcessor**：销毁并释放资源。
- **GetProcessedImage**：获取并返回处理后的图像数据。
- **Scale**：执行图像缩放操作。
- **CropRectangle**：执行图像裁剪操作。
- **SetRotationCenter**：设置图像旋转中心。
- **Rotate**：执行图像旋转操作。
- **Translate**：执行图像平移操作。

## Cpp操作类接口定义
为了在C#中实现类似的功能结构，`ImageProcessor`类提供了与C#接口相似的方法：
- **Create**：为图像处理初始化必要的资源。
- **Process**：根据定义的操作序列执行图像处理。
- **Retrieve**：获取最终的处理结果。

此设计旨在简化C#调用方的思维负担，使其能够以直观的方式调用CPP库中的复杂功能。
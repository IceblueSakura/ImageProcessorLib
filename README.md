# ImageProcessor Cpp
一个C++编写的opencv支持库，主要测试的调用方式是P/Invoke(DLL)

## 调用流程及参数设计
1. [csharp]创建`ImageProcessorWarp`类，并绑定bitmap对象。
2. [csharp]链式调用`ImageProcessorWarp`类中的方法，设计变换流程。
3. [csharp]在`ImageProcessorWarp`类中使用队列记住每次变换的参数。
4. [csharp]调用`ImageProcessorWarp`类中的Commit，在提交后才会进行处理(跨语言内存安全性需求)，这将(copy?)lockbits bitmap，并将锁定后的bitmap指针传递给cpp，构建`ImageProcessor`类。
5. [csharp]按队列形式调用cpp中的方法，执行所有操作。
6. [cpp]在初始化后，将指针区域的长度等用于校验的数据保存(防止内存越界)，并将RAM复制到VRAM。
7. [cpp]之后的变换全部都在VRAM中运行，直到结束所有序列操作后，再将数据从VRAM复制回RAM。
8. [cpp]根据初始化时保存的内存长度等校验信息，校验VRAM传回RAM的数据。
9. [cpp]释放VRAM，但不要释放RAM(因为控制权即将转回csharp)。
10. [csharp]Commit执行结束，unlockbits bitmap，将内存中修改应用到图像中。
11. 处理流程结束

## 类结构设计
- [csharp]因为需要以队列形式记录操作序列最终统一提交，所以设计链式调用的`ImageProcessorWarp`类。
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

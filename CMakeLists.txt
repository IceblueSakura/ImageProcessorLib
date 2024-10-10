# 设置项目名称和版本
cmake_minimum_required(VERSION 3.28)
project(ImageProcessorLib VERSION 1.0 LANGUAGES CXX CUDA)

# 设置CMake策略
cmake_policy(SET CMP0146 OLD)

# 设置C++标准为C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置CUDA库路径
set(CMAKE_PREFIX_PATH /usr/local/cuda)

# CUDA库为必须，并输出详细库信息
find_package(CUDA REQUIRED)
message(STATUS "CUDA library status:")
message(STATUS "    version: ${CUDA_VERSION}")
message(STATUS "    include path: ${CUDA_INCLUDE_DIRS}")

# OpenCV库为必须，并输出详细库信息
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# 添加头文件搜索路径
include_directories(
        ${OpenCV_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
)

# 定义源文件
set(SOURCES
        ${CMAKE_SOURCE_DIR}/src/ImageProcessor.cpp
        ${CMAKE_SOURCE_DIR}/src/ImageProcessorWrapper.cpp
)

# 将库编译为共享库 (DLL)
add_library(ImageProcessorLib SHARED ${SOURCES})

# 链接OpenCV库到项目中
target_link_libraries(ImageProcessorLib ${OpenCV_LIBS})

# 设置生成的库文件名
set_target_properties(ImageProcessorLib PROPERTIES
        OUTPUT_NAME "ImageProcessorLib"       # DLL的名称
        PREFIX ""                             # 取消默认的lib前缀
)


# 针对Windows平台的设置
if (WIN32)
    target_compile_definitions(ImageProcessorLib PRIVATE "BUILDING_DLL")
    set_target_properties(ImageProcessorLib PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    )
endif ()


# 安装目标（可选）
install(TARGETS ImageProcessorLib
        LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib)

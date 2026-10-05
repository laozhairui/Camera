# Camera

一个使用 C++、CMake 和 OpenCV 的相机取图示例项目，采用统一 `Camera` 接口，并提供两种可选后端：

- `OPENCV`：通过 OpenCV `VideoCapture` 访问普通摄像头，适合 Windows 摄像头功能验证。
- `MVS`：通过海康 MVS SDK 访问支持的 GigE/USB 工业相机。

> 重要：MVS 后端需要安装匹配版本的海康 MVS SDK，并且真实设备测试需要海康相机。没有硬件时，不应声称已完成真实海康相机验证。

## 功能

- 打开相机、连续获取图像、关闭相机
- 统一的 C++ `Camera` 接口
- 后端与上层调用分离
- 检查打开/取图失败并返回错误信息
- 析构时自动释放资源
- 按 `S` 保存当前帧为 `capture.jpg`
- 按 `ESC` 或 `Q` 退出

## 项目结构

```text
Camera/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   └── Camera.h
├── src/
│   ├── Camera.cpp
│   ├── CameraBackend.h
│   ├── OpenCVBackend.cpp
│   ├── MVSBackend.cpp
│   └── main.cpp
└── docs/
    ├── DESIGN.md
    └── TEST_REPORT.md
```

## 环境依赖

- C++17 编译器
- CMake 3.22 或更高版本
- OpenCV（core、videoio、highgui、imgcodecs）
- 使用 MVS 后端时还需要海康 MVS SDK

## Windows：普通摄像头（OpenCV）

在已配置好 OpenCV 和 MinGW/CMake 的 PowerShell 中：

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCAMERA_BACKEND=OPENCV -DOpenCV_DIR="C:/opencv_build"
cmake --build build -j4
.\build\Camera.exe
```

如果你的 OpenCV 没有可供 CMake 使用的 `OpenCVConfig.cmake`，需要先正确配置/安装 OpenCV 的 CMake package，或按本机实际路径设置 `OpenCV_DIR`。不要照抄不存在的路径。

## Ubuntu：普通摄像头（OpenCV）

```bash
sudo apt update
sudo apt install build-essential cmake libopencv-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCAMERA_BACKEND=OPENCV
cmake --build build -j"$(nproc)"
./build/Camera
```

## Ubuntu：海康 MVS 后端

确认 MVS SDK 的头文件和动态库路径。此项目默认查找 `/opt/MVS/include`、`/opt/MVS/lib/64` 和 `/opt/MVS_Runtime/MVS/lib/64`。

```bash
cmake -S . -B build-mvs -DCMAKE_BUILD_TYPE=Release \
  -DCAMERA_BACKEND=MVS -DMVS_ROOT=/opt/MVS
cmake --build build-mvs -j"$(nproc)"
./build-mvs/Camera
```

如果 SDK 动态库不在默认路径，使用 `-DMVS_ROOT=/你的/MVS/安装目录`，并按实际情况配置运行时库搜索路径，例如：

```bash
export LD_LIBRARY_PATH=/opt/MVS_Runtime/MVS/lib/64:$LD_LIBRARY_PATH
```

运行前请确认相机已连接、供电正常，并且当前用户具有访问 USB/GigE 设备的权限。

## 操作

- 实时显示图像
- `S`：保存当前帧为 `capture.jpg`
- `ESC` 或 `Q`：退出并释放相机

## 统一接口示例

```cpp
Camera camera;
if (!camera.open(0)) {
    std::cerr << camera.lastError() << '\n';
    return 1;
}

cv::Mat frame;
if (camera.getImage(frame)) {
    cv::imwrite("capture.jpg", frame);
}
camera.close();
```

## 当前限制与诚实说明

- OpenCV 后端已用于普通 Windows 摄像头功能验证（请以自己实际运行记录为准）。
- MVS 后端代码依据 MVS SDK 常见接口组织，必须使用本机安装的 SDK 头文件/库编译；不同 SDK 版本可能存在接口或结构体差异。
- 没有真实海康相机时，无法验证设备枚举、实际取流、像素格式、曝光/增益参数和长期稳定性。
- 当前版本为单线程采集，不宣称已经实现多线程。
- 当前版本没有实现曝光、增益、帧率等相机参数配置。

## Git 提交建议

每完成一项独立功能就单独提交，例如：

```bash
git status
git add CMakeLists.txt include src
git commit -m "refactor: separate camera backend interface"
git push
```

文档可单独提交：

```bash
git add README.md docs
git commit -m "docs: add camera usage and design documents"
git push
```

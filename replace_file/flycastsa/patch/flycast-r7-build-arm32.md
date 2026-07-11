# Flycast ARM32 交叉编译指南

## 概述

本文档描述如何在 x86_64 Linux 主机上交叉编译 Flycast 模拟器，目标平台为 ARM32 (armhf)。

## 工具链

### 交叉编译器

使用 ARM 官方 GCC 9.2 工具链：

```
/opt/toolchains/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/
```

工具链前缀：`arm-none-linux-gnueabihf-`

### 系统根目录 (sysroot)

目标平台的库和头文件位于：

```
/opt/sysroot32/
```

目录结构：
```
/opt/sysroot32/
├── lib/                           # 系统库
│   └── arm-linux-gnueabihf/       # ARM 硬浮点库
├── usr/
│   ├── include/                   # 头文件
│   └── lib/
│       ├── arm-linux-gnueabihf/   # 用户库
│       └── libSDL2.so             # SDL2 库
└── ...
```

## CMake 工具链文件

`arm32-toolchain.cmake`：

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX /opt/toolchains/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}ar)
set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}ranlib)
set(CMAKE_STRIP ${TOOLCHAIN_PREFIX}strip)

set(CMAKE_SYSROOT /opt/sysroot32)

set(CMAKE_FIND_ROOT_PATH /opt/sysroot32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_EXE_LINKER_FLAGS "-L/opt/sysroot32/lib -L/opt/sysroot32/usr/lib/arm-linux-gnueabihf -Wl,-rpath-link,/opt/sysroot32/lib -Wl,-rpath-link,/opt/sysroot32/usr/lib/arm-linux-gnueabihf")
set(CMAKE_SHARED_LINKER_FLAGS "-L/opt/sysroot32/lib -L/opt/sysroot32/usr/lib/arm-linux-gnueabihf -Wl,-rpath-link,/opt/sysroot32/lib -Wl,-rpath-link,/opt/sysroot32/usr/lib/arm-linux-gnueabihf")
```

### 关键参数说明

| 参数 | 值 | 说明 |
|------|-----|------|
| `CMAKE_SYSTEM_NAME` | Linux | 目标操作系统 |
| `CMAKE_SYSTEM_PROCESSOR` | arm | 目标处理器架构 |
| `CMAKE_SYSROOT` | /opt/sysroot32 | 目标系统根目录 |
| `CMAKE_FIND_ROOT_PATH_MODE_PROGRAM` | NEVER | 不在 sysroot 中查找程序 |
| `CMAKE_FIND_ROOT_PATH_MODE_LIBRARY` | ONLY | 只在 sysroot 中查找库 |
| `CMAKE_FIND_ROOT_PATH_MODE_INCLUDE` | ONLY | 只在 sysroot 中查找头文件 |

## 编译选项

### 编译器标志

从 `CMakeCache.txt` 中提取的实际编译标志：

```
CMAKE_C_FLAGS:   -mcpu=cortex-a35 -O2 -ftree-vectorize -marm -mfpu=neon-fp-armv8 -mfloat-abi=hard
CMAKE_CXX_FLAGS: -mcpu=cortex-a35 -O2 -ftree-vectorize -marm -mfpu=neon-fp-armv8 -mfloat-abi=hard
```

| 标志 | 说明 |
|------|------|
| `-mcpu=cortex-a35` | 目标 CPU 为 Cortex-A35 |
| `-marm` | 使用 ARM 指令集（非 Thumb） |
| `-mfpu=neon-fp-armv8` | 使用 NEON 浮点单元 |
| `-mfloat-abi=hard` | 使用硬浮点 ABI |
| `-O2` | 优化级别 2 |
| `-ftree-vectorize` | 启用树向量化 |

### CMake 选项

| 选项 | 值 | 说明 |
|------|-----|------|
| `CMAKE_BUILD_TYPE` | Release | 发布版本 |
| `USE_GLES` | ON | 使用 OpenGL ES 3 |
| `USE_GLES2` | OFF | 不使用 OpenGL ES 2 |
| `USE_VULKAN` | OFF | 不使用 Vulkan |
| `USE_OPENMP` | ON | 启用 OpenMP |
| `USE_HOST_LIBZIP` | ON | 使用主机 libzip |

## 链接库

### 系统库（来自 sysroot）

| 库 | 路径 | 用途 |
|----|------|------|
| SDL2 | `/opt/sysroot32/usr/lib/libSDL2.so` | 输入/音频/窗口管理 |
| zlib | `/opt/sysroot32/usr/lib/arm-linux-gnueabihf/libz.so` | 压缩 |
| libgomp | `/opt/sysroot32/usr/lib/libgomp.so` | OpenMP 并行计算 |
| libpthread | `/opt/sysroot32/usr/lib/libpthread.so` | 线程 |
| libdl | (系统) | 动态链接 |
| librt | (系统) | 实时扩展 |

### 静态库（编译时生成）

| 库 | 路径 | 用途 |
|----|------|------|
| xxhash | `core/deps/xxHash/cmake_unofficial/libxxhash.a` | 哈希算法 |
| miniupnpc | `core/deps/miniupnpc/libminiupnpc.a` | UPnP 网络穿透 |
| libzip | `core/deps/libzip/lib/libzip.a` | ZIP 文件处理 |
| glslang | `core/deps/glslang/*/lib*.a` | GLSL 着色器编译 |
| SPIRV | `core/deps/glslang/SPIRV/libSPIRV.a` | SPIR-V 处理 |

### 链接器标志

```
-L/opt/sysroot32/lib
-L/opt/sysroot32/usr/lib/arm-linux-gnueabihf
-Wl,-rpath-link,/opt/sysroot32/lib
-Wl,-rpath-link,/opt/sysroot32/usr/lib/arm-linux-gnueabihf
```

`-Wl,-rpath-link` 用于在链接时查找共享库的依赖库。

## 编译步骤

### 1. 准备环境

确保工具链和 sysroot 已正确安装：

```bash
# 检查工具链
ls /opt/toolchains/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin/

# 检查 sysroot
ls /opt/sysroot32/usr/lib/libSDL2.so
```

### 2. 配置 CMake

```bash
cd /home/lcdyk/flycast
cmake -B build-arm32 \
    -DCMAKE_TOOLCHAIN_FILE=arm32-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DUSE_GLES=ON \
    -DUSE_VULKAN=OFF \
    -DUSE_OPENMP=ON \
    -DUSE_HOST_LIBZIP=ON
```

### 3. 编译

```bash
cmake --build build-arm32 -j$(nproc)
```

### 4. 输出文件

编译产物位于：

```
build-arm32/flycast
```

## 应用补丁

### 中文支持补丁

```bash
cd /home/lcdyk/flycast
git apply --exclude="core/rend/i18n.h" flycast-chinese-support.patch
```

补丁内容：
- 添加 `i18n.h` 国际化头文件
- 修改字体大小从 17px 到 20px
- 添加中文界面翻译

### 720x720 屏幕适配补丁

```bash
cd /home/lcdyk/flycast
git apply flycast-720x720-fix.patch
```

补丁内容：
- 修复非 4:3 屏幕的画面显示
- 实现等比缩放 + 居中显示
- 上下/左右添加黑边而非裁剪画面

## 架构特定代码

Flycast 根据 `CMAKE_SYSTEM_PROCESSOR` 选择不同的代码路径：

### ARM32 (`arm`)

- `core/rec-ARM/rec_arm.cpp` - ARM32 动态重编译器
- `core/deps/vixl/aarch32/` - VIXL ARM32 汇编器
- `core/hw/arm7/arm7_rec_arm32.cpp` - ARM7 核心重编译

### 渲染器选择

| 选项 | 渲染器 | 说明 |
|------|--------|------|
| `USE_GLES=ON` | OpenGL ES 3 | 默认，适合嵌入式设备 |
| `USE_GLES2=ON` | OpenGL ES 2 | 兼容旧设备 |
| `USE_VULKAN=ON` | Vulkan | 高性能，需要驱动支持 |

## 目录结构

```
flycast/
├── arm32-toolchain.cmake      # 工具链文件
├── CMakeLists.txt             # 主构建文件
├── build-arm32/               # 构建目录
│   ├── flycast                # 编译产物
│   └── CMakeCache.txt         # CMake 缓存
├── core/
│   ├── rend/
│   │   ├── gles/              # OpenGL ES 渲染器
│   │   │   ├── gles.cpp       # 主渲染逻辑
│   │   │   └── gldraw.cpp     # 绘制和输出
│   │   ├── transform_matrix.h # 变换矩阵计算
│   │   ├── gui.cpp            # GUI 界面
│   │   └── i18n.h             # 国际化
│   └── deps/                  # 第三方依赖
└── shell/                     # 平台特定代码
```

## 常见问题

### Q: 编译时找不到 SDL2

A: 确保 sysroot 中有 SDL2 开发文件：

```bash
ls /opt/sysroot32/usr/include/SDL2/SDL.h
ls /opt/sysroot32/usr/lib/libSDL2.so
```

### Q: 链接时找不到库

A: 检查 `CMAKE_EXE_LINKER_FLAGS` 中的库路径是否正确。

### Q: 运行时缺少共享库

A: 在目标设备上安装必要的运行时库：

```bash
apt-get install libsdl2-2.0-0 libgles2 libopenmp1
```

### Q: Vulkan 编译失败

A: 确保目标设备有 Vulkan 驱动和开发文件，或禁用 Vulkan：

```bash
cmake -DUSE_VULKAN=OFF ...
```

## 参考资料

- [Flycast GitHub](https://github.com/flyinghead/flycast)
- [ARM 工具链下载](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a)
- [CMake 交叉编译文档](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)

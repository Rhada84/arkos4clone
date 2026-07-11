# Yabasanshiro 交叉编译文档

## 版本信息

| 版本 | 源码分支 | 二进制名 | 存档路径 |
|------|----------|----------|----------|
| 1.11 | `m2_1_11` | `yabasanshiro-2412` | `/roms(2)/saturn/yabasanshiro-2412/` |
| pi4 | `pi4` | `yabasanshiro-pi4` | `/roms(2)/saturn/yabasanshiro-pi4/` |
| pi4-1-9-0 | `pi4-1-9-0` | `yabasanshiro` | `/roms(2)/saturn/yabasanshiro/` |

## 统一配置

| 项目 | 路径 |
|------|------|
| 按键配置 | `/opt/yabasanshiro/input.cfg` |
| 按键映射 | `/opt/yabasanshiro/keymapv2.json` |
| CJK字体 | `/opt/yabasanshiro/wqy-microhei.ttf` |
| 游戏配置 | `/opt/yabasanshiro/` |

## 源码仓库

- **仓库**: https://github.com/devmiyax/yabause
- **pi4-1-9-0分支**: `pi4-1-9-0`
- **pi4分支**: `pi4`
- **1.11分支**: `m2_1_11` (tag: `1.11.0-beta3`)

## 交叉编译环境

| 项目 | 路径/版本 |
|------|-----------|
| 工具链 | `gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu` |
| Sysroot | `/home/lcdyk/ppsspp_cross` |
| 目标架构 | aarch64 (ARM 64-bit) |
| 目标平台 | RK3326 (Cortex-A35) |
| 主机系统 | Ubuntu 22.04 x86_64 |

## 交叉编译修复

### 1. CMake交叉编译支持

**文件**: `CMake/Packages/external_libchdr.cmake`, `external_libpng.cmake`, `external_zlib.cmake`

**问题**: ExternalProject使用host编译器而非交叉编译器

**修复**: 在`else()`分支添加交叉编译参数
```cmake
else()
  set(ADDITIONAL_CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_SYSROOT=${CMAKE_SYSROOT}
  )
endif()
```

### 2. bin2c交叉编译

**文件**: `src/retro_arena/nanogui-sdl/CMakeLists.txt`

**问题**: bin2c是构建时工具，需要用host编译器编译，但CMake会用交叉编译器编译

**修复**: 检测交叉编译时使用预编译的bin2c
```cmake
if(CMAKE_CROSSCOMPILING)
  set(bin2c_cmdline ${CMAKE_CURRENT_SOURCE_DIR}/bin2c)
else()
  add_executable(bin2c resources/bin2c.c)
  set(bin2c_cmdline ./bin2c)
endif()
```

### 3. libpng NEON禁用

**文件**: `src/retro_arena/png/src/png/pngpriv.h`

**问题**: libpng在ARM架构上自动启用NEON，但交叉编译环境不支持

**修复**: 在文件开头强制定义
```c
#define PNG_ARM_NEON_OPT 0
#if 0
// 原始的 #ifndef PNG_ARM_NEON_OPT ...
```

同时修改 `pngrtran.c` 和 `pngrutil.c`，注释掉NEON函数调用。

### 4. mini18n禁用

**文件**: `src/CMakeLists.txt`

**问题**: mini18n库与sysroot的wchar.h不兼容（locale_t类型冲突）

**修复**: 
```cmake
# 注释掉mini18n定义
# add_definitions(-DHAVE_LIBMINI18N=1)

# 注释掉mini18n源文件
# ${MINI18N_PATH}/mini18n.c
# ${MINI18N_PATH}/mini18n-multi.c
# ...
```

### 5. threads.h u64类型定义

**文件**: `src/threads.h`

**问题**: 缺少u64类型的定义

**修复**: 添加头文件和类型定义
```c
#include <stdint.h>
typedef uint64_t u64;
```

### 6. librga.so符号链接

**问题**: sysroot里的librga.so是指向libMali.so的stub

**修复**: 替换为正确版本的librga.so（无SONAME）

## Patch说明

### 共用修改（三个版本都有）

| 修改 | 文件 | 说明 |
|------|------|------|
| i18n中文菜单 | `MenuScreen.cpp`, `i18n.h` | 根据LANG/LC_ALL自动切换中英文 |
| CJK字体加载 | `nanogui-sdl/src/theme.cpp` | 从二进制目录加载wqy-microhei.ttf |
| 居中显示 | `vidogl.c` | 修复正方形屏幕过裁剪问题 |
| 存档路径 | `main.cpp` | 统一存档路径 |
| input.cfg路径 | `InputManager.cpp` | 读取/opt/yabasanshiro/input.cfg |
| 路径修改 | `main.cpp`, `Preference.cpp` | 配置文件路径修改 |
| 按键映射 | `InputManager.cpp` | 按键配置调整 |
| 低分辨率 | `nx/main.cpp`, `sdl/main.c` | 默认低分辨率模式 |
| 移除GLES3扩展 | `ygl.h`, `nanovg_osdcore.c` | 兼容性修复 |
| 移除关于页面 | `MenuScreen.cpp` | 注释掉About窗口 |
| 存档路径修改 | `main.cpp` | 存档路径调整 |

### 存档路径

| 版本 | 路径 |
|------|------|
| 1.11 | `/roms(2)/saturn/yabasanshiro-2412/` |
| pi4 | `/roms(2)/saturn/yabasanshiro-pi4/` |
| pi4-1-9-0 | `/roms(2)/saturn/yabasanshiro/` |

## 编译步骤

### 1. 准备工具

```bash
# bin2c (x86_64 host工具)
cp yabasanshiro-build-host/bin2c/bin2c yabause/src/retro_arena/nanogui-sdl/

# m68kmake (x86_64 host工具)
mkdir -p build/src/musashi
cp yabasanshiro-build-host/musashi/m68kmake build/src/musashi/
```

### 2. 应用Patch

```bash
cd yabause
patch -Np2 < yabasanshirosa.patch
cp yabasanshirosa-i18n.h src/retro_arena/i18n.h
```

### 3. CMake配置

```bash
cmake -S yabause \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
    -DCMAKE_C_COMPILER=${TOOLCHAIN}/bin/aarch64-linux-gnu-gcc \
    -DCMAKE_CXX_COMPILER=${TOOLCHAIN}/bin/aarch64-linux-gnu-g++ \
    -DCMAKE_SYSROOT=${SYSROOT} \
    -DCMAKE_C_FLAGS="-march=armv8-a+crc -mtune=cortex-a35 -ftree-vectorize -funsafe-math-optimizations -O2" \
    -DCMAKE_CXX_FLAGS="-march=armv8-a+crc -mtune=cortex-a35 -ftree-vectorize -funsafe-math-optimizations -O2" \
    -DCMAKE_EXE_LINKER_FLAGS="--sysroot=${SYSROOT} -L${SYSROOT}/usr/lib/aarch64-linux-gnu -lEGL -lGLESv2 -lSDL2 -lX11 -lXrandr -lopenal -lboost_system -lboost_filesystem -lboost_date_time -lboost_locale -lz -lm -lpthread -lasound -lrga -lstdc++fs -Wl,--allow-multiple-definition" \
    -DYAB_PORTS=retro_arena \
    -DYAB_WANT_DYNAREC_DEVMIYAX=ON \
    -DYAB_WANT_ARM7=ON \
    -DYAB_WANT_VULKAN=OFF \
    -DUSE_EGL=ON \
    -B build
```

### 4. 编译

```bash
cd build
make -j$(nproc)
```

### 5. 打包

```bash
${TOOLCHAIN}/bin/aarch64-linux-gnu-strip src/retro_arena/yabasanshiro
cp src/retro_arena/yabasanshiro yabasanshirosa_pkg/yabasanshiro
```

## 设备部署

```bash
# 复制二进制
cp yabasanshiro /opt/yabasanshiro/
cp yabasanshiro-pi4 /opt/yabasanshiro/
cp yabasanshiro-2412 /opt/yabasanshiro/

# 复制配置文件
cp wqy-microhei.ttf /opt/yabasanshiro/
cp input.cfg /opt/yabasanshiro/
cp keymapv2.json /opt/yabasanshiro/

# 创建存档目录
mkdir -p /roms/saturn/yabasanshiro
mkdir -p /roms/saturn/yabasanshiro-pi4
mkdir -p /roms/saturn/yabasanshiro-2412

# 设置中文locale（可选）
sudo locale-gen zh_CN.UTF-8
export LANG=zh_CN.UTF-8
export LC_ALL=zh_CN.UTF-8
```

## 按键说明

| 按键 | 功能 |
|------|------|
| Select (id=12) | 打开/关闭菜单 |
| Start (id=13) | 功能键 |
| A/B/X/Y/L/R | 游戏按键 |
| 方向键 | 方向 |

## 已知问题

1. **mini18n禁用** - 因为与sysroot的wchar.h不兼容
2. **NEON禁用** - libpng的NEON在交叉编译时不支持
3. **PulseAudio未启用** - 依赖链太复杂，使用ALSA替代
4. **1.11版本** - m2_1_11分支主要是Windows版本，需要额外修复

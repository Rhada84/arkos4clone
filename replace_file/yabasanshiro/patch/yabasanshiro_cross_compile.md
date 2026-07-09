# Yabasanshiro 交叉编译文档

## 概述

Yabasanshiro 是一个世嘉土星(Sega Saturn)模拟器，本文档介绍如何在x86_64主机上交叉编译aarch64版本。

## 项目信息

| 项目 | 内容 |
|------|------|
| 源码 | https://github.com/devmiyax/yabause |
| 分支/标签 | pi4-1-9-0 |
| 目标架构 | aarch64 (ARM 64-bit) |
| 目标平台 | RK3326 (Cortex-A35) |

## 依赖

### 主机依赖

```bash
# 交叉编译工具链
/opt/toolchains/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu

# 构建工具
cmake
git
python3 (用于fonttools转换字体)
```

### 目标依赖 (sysroot)

```
/home/lcdyk/r.arkos/core_cross_build/ (或 /home/lcdyk/ppsspp_cross/)
├── usr/lib/aarch64-linux-gnu/
│   ├── libEGL.so → libMali.so
│   ├── libGLESv2.so → libMali.so
│   ├── libgbm.so → libMali.so
│   ├── libSDL2.so
│   ├── libX11.so
│   ├── libXrandr.so
│   ├── libopenal.so
│   ├── libboost_system.so
│   ├── libboost_filesystem.so
│   ├── libboost_date_time.so
│   ├── libboost_locale.so
│   ├── libasound.so
│   ├── librga.so
│   └── libmali-bifrost-g31-rxp0-gbm.so
└── usr/include/
    ├── SDL2/
    ├── EGL/
    ├── GLES2/
    ├── GLES3/
    └── rga/
```

## 编译步骤

### 1. 克隆源码

```bash
git clone --recursive https://github.com/devmiyax/yabause -b pi4-1-9-0 yabasanshiro
```

### 2. 应用补丁

```bash
cd yabasanshiro

# 应用原有补丁
for patch in ../patches/yabasanshirosa-patch-00{01,02,03,04,05,06,08,09}*.patch; do
    patch -Np1 < "$patch"
done

# 应用完整补丁（包含i18n、居中显示、编译修复）
patch -Np1 < ../patches/yabasanshirosa-patch-0010-i18n-center-cjk.patch
```

### 3. 构建m68kmake (host工具)

```bash
mkdir -p ../yabasanshiro-build-host/musashi
cd ../yabasanshiro-build-host/musashi
cmake ../../yabasanshiro/yabause/src/musashi -G "Unix Makefiles"
make -j$(nproc)
cd ../../yabasanshiro
```

### 4. 配置CMake工具链

创建 `aarch64-toolchain.cmake`:

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER /opt/toolchains/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /opt/toolchains/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++)
set(CMAKE_SYSROOT /home/lcdyk/r.arkos/core_cross_build)
set(CMAKE_FIND_ROOT_PATH /home/lcdyk/r.arkos/core_cross_build)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
```

### 5. 构建

```bash
SYSROOT="/home/lcdyk/ppsspp_cross"
TOOLCHAIN_DIR="/opt/toolchains/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu"

mkdir -p yabasanshiro-build && cd yabasanshiro-build

# 复制m68kmake
mkdir -p src/musashi
cp ../yabasanshiro-build-host/musashi/m68kmake src/musashi/
chmod +x src/musashi/m68kmake

# 配置
cmake -S ../yabasanshiro/yabause \
    -DCMAKE_RULE_MESSAGES=OFF \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
    -DCMAKE_C_COMPILER=${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-gcc \
    -DCMAKE_CXX_COMPILER=${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-g++ \
    -DCMAKE_SYSROOT=${SYSROOT} \
    -DCMAKE_C_FLAGS="-march=armv8-a+crc -mtune=cortex-a35 -ftree-vectorize -funsafe-math-optimizations -O2" \
    -DCMAKE_CXX_FLAGS="-march=armv8-a+crc -mtune=cortex-a35 -ftree-vectorize -funsafe-math-optimizations -O2" \
    -DCMAKE_EXE_LINKER_FLAGS="-L${SYSROOT}/usr/lib/aarch64-linux-gnu -lEGL -lGLESv2 -lSDL2 -lX11 -lXrandr -lopenal -lboost_system -lboost_filesystem -lboost_date_time -lboost_locale -lz -lm -lpthread -lasound -lrga -lstdc++fs -Wl,--allow-multiple-definition" \
    -DYAB_PORTS=retro_arena \
    -DYAB_WANT_DYNAREC_DEVMIYAX=ON \
    -DYAB_WANT_ARM7=ON \
    -DYAB_WANT_VULKAN=OFF \
    -DUSE_EGL=ON \
    -B .

# 编译
make -j$(nproc)

# 打包
${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-strip src/retro_arena/yabasanshiro
cp src/retro_arena/yabasanshiro ../yabasanshirosa_pkg/
```

## 补丁说明

### 0001-0009: 原有补丁

| 补丁 | 说明 |
|------|------|
| 0001 | 添加缺失的include |
| 0002 | 低分辨率模式 |
| 0003 | 移除GL3扩展 |
| 0004 | 修改路径 |
| 0005 | 存档路径 |
| 0006 | 移除关于页面 |
| 0007 | 缩放菜单UI (odroidgoa) |
| 0008 | 禁用SH2 |
| 0009 | 增大GLUI尺寸 |

### 0010: 完整补丁 (i18n + 居中显示 + CJK字体 + 编译修复)

包含内容：

1. **编译修复** - CMake配置、头文件修复
2. **居中显示** - 游戏画面在屏幕上居中显示
3. **i18n支持** - 根据LANG/LC_ALL环境变量自动切换中英文
4. **CJK字体** - 从二进制目录加载wqy-microhei.ttf字体

#### 修改的文件

| 文件 | 修改内容 |
|------|----------|
| `CMake/Packages/external_*.cmake` | 编译修复 |
| `src/retro_arena/MenuScreen.cpp` | i18n中文翻译 |
| `src/retro_arena/i18n.h` | 新增：i18n头文件 |
| `src/retro_arena/nanogui-sdl/src/theme.cpp` | CJK字体加载 |
| `src/vidogl.c` | 居中显示 |
| `src/ygl.h` | 编译修复 |
| `src/threads.h` | 编译修复 |
| 其他 | 各种编译兼容性修复 |

## 设备部署

```bash
# 复制二进制文件
cp yabasanshiro /opt/yabasanshiro/

# 复制CJK字体 (WenQuanYi Micro Hei, TrueType格式)
cp wqy-microhei.ttf /opt/yabasanshiro/

# 设置中文locale (可选)
sudo locale-gen zh_CN.UTF-8
export LANG=zh_CN.UTF-8
export LC_ALL=zh_CN.UTF-8

# 运行
cd /opt/yabasanshiro
./yabasanshiro
```

## 字体说明

NotoSansCJK 使用 CFF (Compact Font Format) 轮廓，stb_truetype 不支持。
必须使用 TrueType 格式的字体，如：
- WenQuanYi Micro Hei (wqy-microhei.ttf)
- Droid Sans Fallback

### 字体转换

```bash
# 从 .ttc 转换为 .ttf
pip install fonttools
python3 -c "
from fontTools.ttLib import TTCollection
ttc = TTCollection('NotoSansCJK-Regular.ttc')
ttc.fonts[0].save('NotoSansCJK-Regular.ttf')
"
```

## 注意事项

1. **SDL2需要X11支持** - yaba使用SDL2+X11显示，不支持纯GBM/DRM
2. **字体格式** - 必须是TrueType(.ttf)，不能是CFF
3. **编译顺序** - 必须先编译m68kmake(host工具)，再编译yabasanshiro
4. **m68kmake** - 需要复制到build目录的src/musashi/下

## 脚本位置

```
rk3326_core_builds/scripts/yabasanshirosa_cross.sh
rk3326_core_builds/patches/
├── yabasanshirosa-patch-0001-add-missing-include.patch
├── yabasanshirosa-patch-0002-low-res-mode.patch
├── ...
├── yabasanshirosa-patch-0010-i18n-center-cjk.patch  (完整补丁)
└── yabasanshirosa-i18n.h
```

## 与原生编译对比

| 项目 | 原生编译 | 交叉编译 |
|------|----------|----------|
| 编译器 | 系统gcc | gcc-linaro-7.5.0 |
| 依赖 | apt install | sysroot已有 |
| 输出大小 | ~4.3MB | 4.3MB |
| 功能 | 完整 | 完整 (含i18n) |
| CJK字体 | 无 | wqy-microhei.ttf |

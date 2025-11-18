#!/bin/bash
# PlatformIO项目设置脚本
# 用于在拉取上游更新后恢复PlatformIO项目结构
# 
# 使用方法:
#   1. 拉取上游更新: git pull upstream main
#   2. 运行此脚本: bash setup_platformio.sh
#   3. 编译项目: pio run

set -e  # 遇到错误立即退出

echo "=========================================="
echo "🔧 开始设置PlatformIO项目结构..."
echo "=========================================="

# 1. 创建src目录
echo "📁 创建src目录..."
mkdir -p src

# 2. 移动源文件到src目录
echo "📦 移动源文件到src目录..."

# 检查是否有.ino文件需要重命名
if ls *.ino 1> /dev/null 2>&1; then
    echo "  - 发现.ino文件，重命名为.cpp..."
    for file in *.ino; do
        if [ -f "$file" ]; then
            mv "$file" "${file%.ino}.cpp"
            echo "    ✓ $file -> ${file%.ino}.cpp"
        fi
    done
fi

# 移动所有.cpp和.h文件到src目录
echo "  - 移动.cpp和.h文件..."
moved_count=0
for file in *.cpp *.h; do
    if [ -f "$file" ]; then
        mv "$file" src/
        moved_count=$((moved_count + 1))
    fi
done
echo "    ✓ 已移动 $moved_count 个文件到src目录"

# 3. 修复MonoWedge.h中的bug (void函数不应该有return值)
echo "🔧 修复MonoWedge.h中的代码bug..."
if [ -f "src/MonoWedge.h" ]; then
    # 使用sed修复return语句
    sed -i 's/return mono_wedge_update(wedge, value, std::less<T>());/mono_wedge_update(wedge, value, std::less<T>());/g' src/MonoWedge.h
    sed -i 's/return mono_wedge_update(wedge, value, std::greater<T>());/mono_wedge_update(wedge, value, std::greater<T>());/g' src/MonoWedge.h
    echo "  ✓ MonoWedge.h 已修复"
else
    echo "  ⚠️  警告: src/MonoWedge.h 不存在"
fi

# 4. 检查platformio.ini是否存在
echo "📝 检查platformio.ini配置..."
if [ ! -f "platformio.ini" ]; then
    echo "  ⚠️  platformio.ini 不存在，创建新文件..."
    cat > platformio.ini << 'EOF'
;PlatformIO Project Configuration File
;
;   Build options: build flags, source filter
;   Upload options: custom upload port, speed and extra flags
;   Library options: dependencies, extra library storages
;   Advanced options: extra scripting
;
; Please visit documentation for the other options and examples
; https://docs.platformio.org/page/projectconf.html

[env:m5stack-atoms3]
platform = espressif32
board = m5stack-atoms3
framework = arduino

; Serial Monitor options
monitor_speed = 115200
monitor_filters = 
    default
    time

; Build options
build_flags = 
    -DARDUINO_M5STACK_ATOMS3
    -DBOARD_HAS_PSRAM
    -mfix-esp32-psram-cache-issue

; Library dependencies
lib_deps = 
    m5stack/M5Unified@^0.1.16
    hideakitai/ArduinoEigen@^0.3.2

; Extra scripts - 固件合并脚本
extra_scripts = 
    post:merge_firmware.py
EOF
    echo "  ✓ platformio.ini 已创建"
else
    echo "  ✓ platformio.ini 已存在"
fi

# 5. 检查merge_firmware.py是否存在
if [ ! -f "merge_firmware.py" ]; then
    echo "  ⚠️  警告: merge_firmware.py 不存在，请手动添加"
fi

# 6. 显示项目结构
echo ""
echo "=========================================="
echo "✅ PlatformIO项目设置完成！"
echo "=========================================="
echo ""
echo "📂 项目结构:"
echo "  bbn_wave_freq_m5atomS3/"
echo "  ├── platformio.ini       (PlatformIO配置)"
echo "  ├── merge_firmware.py    (固件合并脚本)"
echo "  ├── src/                 (源代码目录)"
echo "  │   ├── *.cpp"
echo "  │   └── *.h"
echo "  ├── data-sim/            (仿真数据)"
echo "  ├── doc/                 (文档)"
echo "  ├── plots/               (绘图脚本)"
echo "  ├── symb-math/           (符号数学)"
echo "  └── tests/               (测试)"
echo ""
echo "🚀 下一步操作:"
echo "  1. 编译项目:   pio run"
echo "  2. 上传固件:   pio run -t upload"
echo "  3. 串口监视:   pio device monitor"
echo ""
echo "📦 固件将生成在:"
echo "  - .pio/build/m5stack-atoms3/"
echo "  - firmware/"
echo ""


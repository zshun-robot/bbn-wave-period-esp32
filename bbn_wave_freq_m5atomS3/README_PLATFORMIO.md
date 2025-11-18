# PlatformIO 项目说明

本项目从上游Arduino项目fork而来，已转换为PlatformIO项目。

## 项目修改

相比原始Arduino项目的主要修改：

1. **项目结构**：所有源代码（`.cpp`和`.h`）移动到`src/`目录
2. **代码修复**：修复了`MonoWedge.h`中void函数的return语句bug
3. **新增文件**：
   - `platformio.ini` - PlatformIO配置
   - `merge_firmware.py` - 固件合并脚本
   - `setup_platformio.sh` / `setup_platformio.bat` - 自动设置脚本

## 快速开始

```bash
# 编译
pio run

# 上传
pio run -t upload

# 串口监视
pio device monitor
```

## 同步上游更新

### 首次设置（仅需一次）
```bash
# 添加上游仓库
git remote add upstream https://github.com/bareboat-necessities/bbn-wave-period-esp32.git
```

### 同步更新流程

#### Windows
```bash
git fetch upstream
git merge upstream/main
setup_platformio.bat
pio run
```

#### Linux/Mac
```bash
git fetch upstream
git merge upstream/main
bash setup_platformio.sh
pio run
```

## 脚本功能

`setup_platformio.sh` / `setup_platformio.bat` 会自动：
- 创建`src/`目录
- 将`.ino`重命名为`.cpp`
- 移动所有源文件到`src/`
- 修复`MonoWedge.h`的bug
- 检查并创建`platformio.ini`

## 固件烧录

### 使用PlatformIO
```bash
pio run -t upload
```

### 使用esptool（全量固件）
```bash
esptool.py --chip esp32s3 --port COM3 --baud 1500000 write_flash 0x0 firmware/esp32s3_m5stack-atoms3_8MB_latest.bin
```

固件文件位置：
- `.pio/build/m5stack-atoms3/` - 构建目录
- `firmware/` - 固件输出目录（带时间戳和latest版本）

## 项目结构

```
bbn_wave_freq_m5atomS3/
├── platformio.ini              # PlatformIO配置
├── merge_firmware.py           # 固件合并脚本
├── setup_platformio.sh         # 设置脚本(Linux/Mac)
├── setup_platformio.bat        # 设置脚本(Windows)
├── src/                        # 源代码目录
│   ├── bbn_wave_freq_m5atomS3.cpp
│   └── *.h
├── firmware/                   # 固件输出
├── data-sim/                   # 仿真数据
├── doc/                        # 文档
├── plots/                      # 绘图脚本
└── tests/                      # 测试
```

## 依赖库

- `m5stack/M5Unified@^0.1.16`
- `hideakitai/ArduinoEigen@^0.3.2`

## 常见问题

**Q: 编译错误找不到ArduinoEigenDense.h**  
A: 确保`platformio.ini`中包含`hideakitai/ArduinoEigen@^0.3.2`

**Q: void函数return错误**  
A: 运行设置脚本修复`MonoWedge.h`

**Q: 找不到源文件**  
A: 确保所有`.cpp`和`.h`文件在`src/`目录中

## 更多信息

- PlatformIO文档：https://docs.platformio.org
- M5Stack文档：https://docs.m5stack.com


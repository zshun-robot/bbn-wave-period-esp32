# PlatformIO 项目说明

本项目从上游Arduino项目fork而来，已转换为PlatformIO项目。

上游仓库：https://github.com/bareboat-necessities/bbn-wave-period-esp32

## 项目修改

相比原始Arduino项目的主要修改：

1. **项目结构**：所有源代码（`.cpp` 和 `.h`）移动到 `src/` 目录
2. **代码修复**：修复了 `MonoWedge.h` 中 void 函数的 return 语句 bug（上游 v7.x -> v8.x 已修复）
3. **新增文件**：
   - `platformio.ini` - PlatformIO 配置
   - `merge_firmware.py` - 固件合并脚本
   - `setup_platformio.sh` / `setup_platformio.bat` - 自动同步脚本

## 快速开始

```bash
# 进入项目目录
cd bbn_wave_freq_m5atomS3

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

### Windows 同步流程

```bash
# 1. 拉取上游更新
git fetch upstream
git merge upstream/main --no-edit -X theirs

# 2. 手动处理 rename/delete 冲突（如有）
#    上游删除的文件（我们 src/ 中存在的）: git rm <file>
#    然后: git commit -m "同步上游更新"

# 3. 恢复PlatformIO结构（将新增文件移入 src/）
cd bbn_wave_freq_m5atomS3
setup_platformio.bat

# 4. 编译测试
pio run

# 5. 提交并推送
git add .
git commit -m "PlatformIO架构更新: 同步上游新增文件"
git push origin refs/heads/main:refs/heads/main
```

### Linux/Mac 同步流程

```bash
git fetch upstream
git merge upstream/main --no-edit -X theirs
cd bbn_wave_freq_m5atomS3
bash setup_platformio.sh
pio run
```

## 脚本功能

`setup_platformio.sh` / `setup_platformio.bat` 会自动：

- 创建 `src/` 和 `include/` 目录
- 将 `.ino` 重命名为 `.cpp`
- 移动所有 `.cpp` 到 `src/`，`.h` 文件到 `include/`
- 修复 `MonoWedge.h` 的 bug（如有）
- 检查并创建 `platformio.ini`

## 固件烧录

### 使用PlatformIO（推荐）

```bash
pio run -t upload
```

### 使用 esptool（全量固件）

```bash
esptool.py --chip esp32s3 --port COM3 --baud 1500000 write_flash 0x0 firmware/esp32s3_m5stack-atoms3_8MB_latest.bin
```

固件文件位置：

- `.pio/build/m5stack-atoms3/` - 构建目录
- `firmware/` - 固件输出目录（带时间戳和 latest 版本）

## 项目结构

```
bbn_wave_freq_m5atomS3/
├── platformio.ini              # PlatformIO配置 (目标板: m5stack-atoms3)
├── merge_firmware.py           # 固件合并脚本
├── setup_platformio.sh         # 设置脚本 (Linux/Mac)
├── setup_platformio.bat        # 设置脚本 (Windows)
├── include/                    # 头文件目录
│   ├── AtomS3R_*.h             # AtomS3R 扩展头文件 (BMI270, 仅供参考)
│   ├── Bosch*.h                # Bosch IMU 驱动头文件 (AtomS3R 专用)
│   └── *.h                     # 通用算法头文件
├── src/                        # 源代码目录
│   ├── bbn_wave_freq_m5atomS3.cpp   # 主程序 (M5AtomS3, MPU6886)
│   ├── ImuCalWizardRunner.cpp  # AtomS3R IMU 校准 (被 build_src_filter 排除)
├── firmware/                   # 固件输出
├── data-sim/                   # 仿真数据
├── doc/                        # 文档
├── plots/                      # 绘图脚本
└── tests/                      # 测试
```

## 依赖库

| 库 | 版本 | 说明 |
|---|---|---|
| `m5stack/M5Unified` | `^0.2.13` | M5Stack 统一驱动库 |
| `hideakitai/ArduinoEigen` | `^0.3.2` | Eigen 线性代数库 (Arduino 版) |

> **注意**: `Arduino_BMI270_BMM150` 库仅在使用 AtomS3R (BMI270 IMU) 时需要，
> 主程序使用 M5AtomS3 内置 MPU6886，通过 M5Unified 自动支持。

## 常见问题

**Q: 编译错误找不到 ArduinoEigenDense.h**  
A: 确保 `platformio.ini` 中包含 `hideakitai/ArduinoEigen@^0.3.2`

**Q: ImuCalWizardRunner.cpp 编译错误**  
A: 该文件是 AtomS3R 专用，已通过 `build_src_filter` 排除，正常编译不会处理它

**Q: 找不到源文件**  
A: 确保所有 `.cpp` 文件在 `src/` 目录，所有 `.h` 文件在 `include/` 目录中，运行 `setup_platformio.bat` 自动整理

**Q: 上游同步后有冲突**  
A: rename/delete 冲突通常是上游删除了文件但我们 `include/` 或 `src/` 中还有，用 `git rm <file>` 接受删除即可

## 版本历史

- 同步到 upstream v8.8.5+（2026-03-11）：新增 BoschBmi270/Bmm150/CalibrateIMU/AtomS3R 系列文件
- v7.3.6（初始版本）：转换为 PlatformIO 项目

## 更多信息

- PlatformIO 文档：https://docs.platformio.org
- M5Stack 文档：https://docs.m5stack.com
- 上游项目：https://github.com/bareboat-necessities/bbn-wave-period-esp32

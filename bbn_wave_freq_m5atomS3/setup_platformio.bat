@echo off
REM PlatformIO项目设置脚本 (Windows版本)
REM 用于在拉取上游更新后恢复PlatformIO项目结构
REM 
REM 使用方法:
REM   1. 拉取上游更新: git fetch upstream && git merge upstream/main
REM   2. 运行此脚本: setup_platformio.bat
REM   3. 编译项目: pio run

setlocal enabledelayedexpansion

echo ==========================================
echo 🔧 开始设置PlatformIO项目结构...
echo ==========================================
echo.

REM 1. 创建src和include目录
echo 📁 创建src和include目录...
if not exist "src" mkdir src
if not exist "include" mkdir include

REM 2. 移动源文件到src目录
echo 📦 移动源文件到src目录...

REM 检查并重命名.ino文件（仅处理主sketch，忽略.ino-draft和.ino-new-draft）
set ino_count=0
for %%f in (*.ino) do (
    if exist "%%f" (
        set /a ino_count+=1
        set "filename=%%~nf"
        ren "%%f" "!filename!.cpp"
        echo   ✓ %%f -^> !filename!.cpp
    )
)
if !ino_count! gtr 0 (
    echo   - 已重命名 !ino_count! 个.ino文件为.cpp
)

REM 移动所有.cpp文件到src目录, .h文件到include目录
set moved_cpp=0
set moved_h=0
for %%f in (*.cpp) do (
    if exist "%%f" (
        move /Y "%%f" "src\" >nul 2>&1
        set /a moved_cpp+=1
    )
)
for %%f in (*.h) do (
    if exist "%%f" (
        move /Y "%%f" "include\" >nul 2>&1
        set /a moved_h+=1
    )
)
echo   ✓ 已移动 !moved_cpp! 个 .cpp 文件到 src 目录，!moved_h! 个 .h 文件到 include 目录

REM 3. 修复MonoWedge.h中的bug（如果还存在）
echo 🔧 检查MonoWedge.h中的代码bug...
if exist "include\MonoWedge.h" (
    powershell -Command "(Get-Content 'include\MonoWedge.h') -replace 'return mono_wedge_update\(wedge, value, std::less<T>\(\)\);', 'mono_wedge_update(wedge, value, std::less<T>());' | Set-Content 'include\MonoWedge.h'"
    powershell -Command "(Get-Content 'include\MonoWedge.h') -replace 'return mono_wedge_update\(wedge, value, std::greater<T>\(\)\);', 'mono_wedge_update(wedge, value, std::greater<T>());' | Set-Content 'include\MonoWedge.h'"
    echo   ✓ MonoWedge.h 已检查并修复
) else (
    echo   ⚠️  警告: include\MonoWedge.h 不存在（或者不需要修复）
)

REM 4. 检查platformio.ini是否存在
echo 📝 检查platformio.ini配置...
if not exist "platformio.ini" (
    echo   ⚠️  platformio.ini 不存在，创建新文件...
    (
        echo ;PlatformIO Project Configuration File
        echo.
        echo [env:m5stack-atoms3]
        echo platform = espressif32
        echo board = m5stack-atoms3
        echo framework = arduino
        echo.
        echo ; Serial Monitor options
        echo monitor_speed = 115200
        echo monitor_filters =
        echo     default
        echo     time
        echo.
        echo ; Build options
        echo build_flags =
        echo     -DARDUINO_M5STACK_ATOMS3
        echo     -DBOARD_HAS_PSRAM
        echo     -mfix-esp32-psram-cache-issue
        echo     -mvector
        echo     -funroll-loops
        echo     -fno-finite-math-only
        echo.
        echo ; Library dependencies
        echo lib_deps =
        echo     m5stack/M5Unified@^0.2.13
        echo     hideakitai/ArduinoEigen@^0.3.2
        echo.
        echo ; 忽略 AtomS3R (Bosch BMI270) 相关文件
        echo build_src_filter =
        echo     +^<*^>
        echo     -^<ImuCalWizardRunner.cpp^>
        echo.
        echo ; Extra scripts - 固件合并脚本
        echo extra_scripts =
        echo     post:merge_firmware.py
    ) > platformio.ini
    echo   ✓ platformio.ini 已创建
) else (
    echo   ✓ platformio.ini 已存在
)

REM 5. 检查merge_firmware.py是否存在
if not exist "merge_firmware.py" (
    echo   ⚠️  警告: merge_firmware.py 不存在，请手动添加
)

REM 6. 显示项目结构
echo.
echo ==========================================
echo ✅ PlatformIO项目设置完成！
echo ==========================================
echo.
echo 📂 项目结构:
echo   bbn_wave_freq_m5atomS3/
echo   ├── platformio.ini       (PlatformIO配置)
echo   ├── merge_firmware.py    (固件合并脚本)
echo   ├── include/             (头文件目录)
echo   │   ├── AtomS3R_*.h      (AtomS3R扩展头文件)
echo   │   ├── Bosch*.h         (Bosch IMU驱动头文件)
echo   │   └── *.h              (通用算法头文件)
echo   ├── src/                 (源代码目录)
echo   │   ├── *.cpp            (实现文件和主程序)
echo   ├── data-sim/            (仿真数据)
echo   ├── doc/                 (文档)
echo   ├── plots/               (绘图脚本)
echo   ├── symb-math/           (符号数学)
echo   └── tests/               (测试)
echo.
echo 🚀 下一步操作:
echo   1. 编译项目:   pio run
echo   2. 上传固件:   pio run -t upload
echo   3. 串口监视:   pio device monitor
echo.
echo 📦 固件将生成在:
echo   - .pio\build\m5stack-atoms3\
echo   - firmware\
echo.
echo 💡 说明:
echo   - 主程序 (bbn_wave_freq_m5atomS3.cpp) 适用于 M5Stack AtomS3 (MPU6886 IMU)
echo   - AtomS3R_* 和 Bosch* 文件为 AtomS3R (BMI270 IMU) 保留，通过 build_src_filter 排除编译
echo.

pause

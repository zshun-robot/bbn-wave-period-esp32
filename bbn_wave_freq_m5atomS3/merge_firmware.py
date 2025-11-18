"""
ESP32-S3 固件合并脚本 - M5Stack AtomS3 波浪周期检测项目
用于将bootloader、分区表、应用程序合并为单个可直接烧录的固件文件
适用于M5Stack AtomS3 (ESP32-S3)

使用方法:
1. 在platformio.ini中添加: extra_scripts = post:merge_firmware.py
2. 执行编译: pio run
3. 固件将生成在 .pio/build/m5stack-atoms3/ 和 firmware/ 目录中

烧录方法:
  esptool.py --chip esp32s3 write_flash 0x0 firmware/esp32s3_bbn_wave_freq_m5atomS3_8MB_latest.bin
"""

Import('env')
import os
import shutil
from datetime import datetime

# 输出目录配置
OUTPUT_DIR = "$BUILD_DIR{}".format(os.path.sep)
APP_BIN = "$BUILD_DIR/${PROGNAME}.bin"

# 固件输出目录（输出到项目根目录的firmware文件夹）
FIRMWARE_OUTPUT_DIR = os.path.join(env.get("PROJECT_DIR"), "firmware")


def get_flash_freq(f_flash):
    """根据flash频率配置返回对应的频率字符串"""
    freq_map = {
        '80000000L': '80m',
        '40000000L': '40m',
        '26000000L': '26m',
        '20000000L': '20m'
    }
    return freq_map.get(f_flash, '40m')


def get_firmware_info(env, board):
    """获取固件信息 - 针对M5Stack AtomS3 (ESP32-S3)优化"""
    mcu = board.get("build.mcu", "esp32s3")
    flash_size = board.get("upload.flash_size", "8MB")
    f_flash = board.get("build.f_flash", "80000000L")
    flash_freq = get_flash_freq(f_flash)
    flash_mode = board.get("build.flash_mode", "dio")

    return {
        'mcu': mcu,
        'flash_size': flash_size,
        'flash_freq': flash_freq,
        'flash_mode': flash_mode
    }


def copy_merge_bins(source, target, env):
    """合并固件文件的主函数 - M5Stack AtomS3波浪周期检测项目"""
    print("\n" + "="*80)
    print("🌊 开始合并M5Stack AtomS3波浪周期检测固件...")
    print("="*80)

    firmware_src = str(target[0])
    flash_images = env.Flatten(env.get("FLASH_EXTRA_IMAGES", [])) + ["$ESP32_APP_OFFSET", APP_BIN]

    # 获取项目名称 (bbn_wave_freq_m5atomS3)
    project_name = env.get("PIOENV", "bbn_wave_freq_m5atomS3")

    # 获取板卡配置
    board = env.BoardConfig()
    info = get_firmware_info(env, board)

    # 生成固件文件名（包含时间戳）
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    firmware_name = "{}_{}_{}_{}.bin".format(
        info['mcu'],
        project_name,
        info['flash_size'],
        timestamp
    )

    # 固件输出路径（在build目录）
    # 需要展开环境变量
    build_dir = env.subst(OUTPUT_DIR)
    firmware_dst = os.path.join(build_dir, firmware_name)

    # 删除旧文件
    if os.path.isfile(firmware_dst):
        os.remove(firmware_dst)
        print("已删除旧固件文件")

    # 构建esptool命令
    cmd = " ".join([
        "$PYTHONEXE",
        "$OBJCOPY",
        '--chip', info['mcu'],
        'merge_bin',
        '--output', firmware_dst,
        '--flash_mode', info['flash_mode'],
        '--flash_size', info['flash_size'],
        '--flash_freq', info['flash_freq']
    ] + flash_images)

    # 执行合并命令
    print("\n执行固件合并命令...")
    env.Execute(cmd)

    # 打印固件信息
    if os.path.isfile(firmware_dst):
        file_size = os.path.getsize(firmware_dst)
        print("\n" + "="*80)
        print("✅ M5Stack AtomS3 波浪周期检测固件合并成功！")
        print("="*80)
        print("项目名称: bbn_wave_freq_m5atomS3")
        print("芯片型号: {}".format(info['mcu'].upper()))
        print("Flash大小: {}".format(info['flash_size']))
        print("Flash频率: {}".format(info['flash_freq']))
        print("Flash模式: {}".format(info['flash_mode']))
        print("固件大小: {:.2f} KB ({} bytes)".format(file_size / 1024, file_size))
        print("固件路径: {}".format(firmware_dst))
        print("\n🔧 烧录命令:")
        print("  esptool.py --chip {} write_flash 0x0 {}".format(
            info['mcu'],
            os.path.basename(firmware_dst)
        ))
        print("\n📝 或使用完整路径:")
        print("  esptool.py --chip {} --port COM3 --baud 1500000 write_flash 0x0 {}".format(
            info['mcu'],
            firmware_dst
        ))

        # 可选：复制到firmware目录
        try:
            if not os.path.exists(FIRMWARE_OUTPUT_DIR):
                os.makedirs(FIRMWARE_OUTPUT_DIR)

            firmware_copy = os.path.join(FIRMWARE_OUTPUT_DIR, firmware_name)
            shutil.copy2(firmware_dst, firmware_copy)
            print("\n固件已复制到: {}".format(firmware_copy))

            # 同时创建一个不带时间戳的latest版本
            latest_name = "{}_{}_{}_latest.bin".format(
                info['mcu'],
                project_name,
                info['flash_size']
            )
            latest_path = os.path.join(FIRMWARE_OUTPUT_DIR, latest_name)
            shutil.copy2(firmware_dst, latest_path)
            print("最新版本: {}".format(latest_path))

        except Exception as e:
            print("\n⚠️  复制固件到firmware目录失败: {}".format(str(e)))

        print("="*80 + "\n")
    else:
        print("\n❌ 固件合并失败！")


# 注册后处理动作
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [copy_merge_bins])
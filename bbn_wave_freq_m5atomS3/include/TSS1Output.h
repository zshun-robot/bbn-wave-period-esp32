#ifndef TSS1Output_h
#define TSS1Output_h

#include <Arduino.h>

/*
  TSS1 / R Heave 1 / R Heave 2 原版协议格式输出
  
  Format (格式): 27-byte 定长字节, 7-bit ASCII
  Message (报文): : XX AAAA S MHHHH Q MRRRR S MPPPP <cr><lf>
  
  字段详解 (Fields Definition):
  - `:`       起始符 (Start character): ASCII冒号 (3Ah)
  - `XX`      水平加速度 (Horizontal acceleration, Local X-Y): 单位为 0.0383 m/s²。ASCII HEX格式输出(0-9, A-Z)，高位在前(MSB first)。无符号数 (0.0 到 9.7665 m/s²)。
  - `AAAA`    垂直加速度 (Vertical acceleration, Local Z): 单位为 0.000625 m/s²。ASCII HEX格式输出(0-9, A-Z)，高位在前。有符号数：向下为负 (-20.48 到 20.48 m/s²)。
  - `S`       空格 (Space): ASCII空格 (20h)
  - `MHHHH`   升沉/垂荡 (Heave): 
                M = 符号位 (Sign): 正数为主数据基准面上方为空格(' ')，负数为减号('-') (Above datum: Positive)
                HHHH = 数值: 单位为 0.01 m，范围 0000 到 9999 (-99.99 到 99.99 m)
  - `Q`       状态标志位 (Status flag): 代表设备当前是否稳定，'U' = 稳定/正常工作，'u' = 未稳定/初始对准中
  - `MRRRR`   横滚角 (Absolute Roll):
                M = 符号位 (Sign): 左舷抬起(Port up)为正(' ')，右舷抬起为负('-')
                RRRR = 数值: 单位为 0.01 度(°)，范围 0000 到 9999 (-99.99° 到 99.99°)
  - `S`       空格 (Space): ASCII空格 (20h)
  - `MPPPP`   俯仰角 (Pitch):
                M = 符号位 (Sign): 船艏仰起(Bow up)为正(' ')，船艏下沉为负('-')
                PPPP = 数值: 单位为 0.01 度(°)，范围 0000 到 9000 (-90.00° 到 90.00°)
  - `<cr><lf>`结尾符 (Terminator pair): 回车+换行 (0Dh, 0Ah)
*/

void gen_tss1_data(float horizontal_accel, float vertical_accel, float heave, 
                   float roll, float pitch, bool is_stable) {
                   
  // XX: Horizontal accel (0 to 9.7665 m/s^2)
  uint8_t xx_val = (uint8_t)constrain(round(abs(horizontal_accel) / 0.0383f), 0, 255);
  
  // AAAA: Vertical accel (-20.48 to 20.48 m/s^2). 
  // Cast to uint16_t for Two's complement HEX output.
  int16_t aaaa_val = (int16_t)constrain(round(vertical_accel / 0.000625f), -32768, 32767);
  
  // MHHHH: Heave (-99.99 to 99.99 m)
  char heave_sign = (heave >= 0) ? ' ' : '-';
  uint16_t hhhh_val = (uint16_t)constrain(round(abs(heave) * 100.0f), 0, 9999);
  
  // Q: Status flag (U = settled, u = unsettled)
  char status = is_stable ? 'U' : 'u';
  
  // MRRRR: Roll (-99.99 to 99.99 deg)
  char roll_sign = (roll >= 0) ? ' ' : '-';
  uint16_t rrrr_val = (uint16_t)constrain(round(abs(roll) * 100.0f), 0, 9999);
  
  // MPPPP: Pitch (-90.00 to 90.00 deg)
  char pitch_sign = (pitch >= 0) ? ' ' : '-';
  uint16_t pppp_val = (uint16_t)constrain(round(abs(pitch) * 100.0f), 0, 9000);
  
  // 按照规范格式严格拼接：
  // : XX AAAA S MHHHH Q MRRRR S MPPPP \r \n
  Serial.printf(":%02X%04X %c%04d%c%c%04d %c%04d\r\n", 
                xx_val, 
                (uint16_t)aaaa_val, 
                heave_sign, hhhh_val, status,
                roll_sign, rrrr_val,
                pitch_sign, pppp_val);
}

void gen_tss1_simple(float vertical_accel, float heave, float roll, float pitch) {
  float horizontal_accel = 0.0f;
  // 默认标志输出为稳定 (U)
  gen_tss1_data(horizontal_accel, vertical_accel, heave, roll, pitch, true);
}

#endif

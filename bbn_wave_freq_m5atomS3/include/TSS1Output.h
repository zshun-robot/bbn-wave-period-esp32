#ifndef TSS1Output_h
#define TSS1Output_h

/*
  TSS1 Data Format Output
  
  TSS1数据格式说明：
  格式示例：":00000A -0002F 0014 -0027"
  - 以":"开始，以空格分割，以回车换行结尾
  - 字段说明：
    1. 00 - 水平加速度（如果未使用为0）
    2. 000A - 垂直加速度（如果未使用为0）  
    3. -0002F - Heave（升沉值）+ 状态标志
    4. 0014 - 横滚 (-99.99~99.99)
    5. -0027 - 俯仰 (-99.99~99.99)
  
  状态标志含义：
  - U = 无辅助模式-已稳定
  - u = 无辅助模式-未稳定，无效数据
  - G = GPS辅助模式-稳定状态
  - g = GPS辅助模式-未稳定，无效数据
  - H = 航向辅助模式-稳定
  - h = 航向辅助模式-未稳定，无效数据
  - F = 全辅助模式-已稳定
  - f = 全辅助模式-未稳定，无效数据
*/

/**
 * 生成TSS1格式数据输出
 * @param horizontal_accel 水平加速度 (m/s²)
 * @param vertical_accel 垂直加速度 (m/s²)
 * @param heave 升沉值 (m)
 * @param roll 横滚角度 (度)
 * @param pitch 俯仰角度 (度)
 * @param is_stable 数据是否稳定
 */
void gen_tss1_data(float horizontal_accel, float vertical_accel, float heave, 
                   float roll, float pitch, bool is_stable) {
  
  // 转换数据为TSS1格式所需的整数值
  int h_accel = (int)(horizontal_accel * 1000);  // 转换为毫g单位
  int v_accel = (int)(vertical_accel * 1000);    // 转换为毫g单位
  int heave_cm = (int)(heave * 100);             // 转换为厘米
  int roll_hundredths = (int)(roll * 100);       // 保留两位小数
  int pitch_hundredths = (int)(pitch * 100);     // 保留两位小数
  
  // 限制数值范围
  h_accel = constrain(h_accel, -99999, 99999);
  v_accel = constrain(v_accel, -99999, 99999);
  heave_cm = constrain(heave_cm, -99999, 99999);
  roll_hundredths = constrain(roll_hundredths, -9999, 9999);
  pitch_hundredths = constrain(pitch_hundredths, -9999, 9999);
  
  // 确定状态标志（始终使用无辅助模式-已稳定）
  char status = 'U';
  
  // 生成TSS1格式字符串
  // 重新分析格式：":000005 0000U 0001 -0024"
  // 字段1: 000005 (水平加速度00 + 垂直加速度0005)
  // 字段2: 0000U (Heave值0000 + 状态标志U)
  // 字段3: 0001 (横滚值，范围-99.99~99.99，所以0001表示0.01度)
  // 字段4: -0024 (俯仰值，范围-99.99~99.99，所以-0024表示-0.24度)

  // TSS1格式：负值直接连接，正值用空格分隔
  // 示例：:000021A-0037F-1369-0293 (全负值)
  // 示例：:000021A 0037F 1369 0293 (全正值)

  // 构建输出字符串
  char output[64];
  char field1[16], field2[16], field3[16], field4[16];

  // 字段1: 水平加速度 + 垂直加速度
  snprintf(field1, sizeof(field1), "%02d%04d",
           abs(h_accel) % 100, abs(v_accel) % 10000);

  // 字段2: Heave + 状态标志
  if (heave_cm >= 0) {
    snprintf(field2, sizeof(field2), "%04d%c", heave_cm, status);
  } else {
    snprintf(field2, sizeof(field2), "-%04d%c", abs(heave_cm) % 10000, status);
  }

  // 字段3: 横滚
  if (roll_hundredths >= 0) {
    snprintf(field3, sizeof(field3), "%04d", roll_hundredths);
  } else {
    snprintf(field3, sizeof(field3), "-%04d", abs(roll_hundredths) % 10000);
  }

  // 字段4: 俯仰
  if (pitch_hundredths >= 0) {
    snprintf(field4, sizeof(field4), "%04d", pitch_hundredths);
  } else {
    snprintf(field4, sizeof(field4), "-%04d", abs(pitch_hundredths) % 10000);
  }

  // TSS1格式：特殊的空格规律
  Serial.print(":");
  Serial.print(field1);  // 第一个字段

  // 第一个字段后面的空格：第二个字段为正数时两个空格，负数时一个空格
  if (heave_cm >= 0) {
    Serial.print("  ");  // 第二个字段为正数时，第一个字段后两个空格
  } else {
    Serial.print(" ");   // 第二个字段为负数时，第一个字段后一个空格
  }

  // 字段2: Heave + 状态标志
  Serial.print(field2);

  // 第二个字段后面的空格：第三个字段为正数时一个空格，负数时没有空格
  if (roll_hundredths >= 0) {
    Serial.print(" ");  // 第三个字段为正数时前面加空格
  }
  // 第三个字段为负数时直接连接，没有空格
  Serial.print(field3);

  // 第三个字段后面的空格：第四个字段为正数时两个空格，负数时一个空格
  if (pitch_hundredths >= 0) {
    Serial.print("  ");  // 第四个字段为正数时前面加两个空格
  } else {
    Serial.print(" ");   // 第四个字段为负数时前面加一个空格
  }
  Serial.print(field4);

  Serial.print("\r\n");
}

/**
 * 简化版TSS1输出函数，始终使用稳定状态
 * @param vertical_accel 垂直加速度 (m/s²)
 * @param heave 升沉值 (m)
 * @param roll 横滚角度 (度)
 * @param pitch 俯仰角度 (度)
 */
void gen_tss1_simple(float vertical_accel, float heave, float roll, float pitch) {
  // 水平加速度暂时设为0
  float horizontal_accel = 0.0;

  // 始终使用稳定状态
  gen_tss1_data(horizontal_accel, vertical_accel, heave, roll, pitch, true);
}

#endif

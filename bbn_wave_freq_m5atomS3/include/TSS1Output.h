#ifndef TSS1Output_h
#define TSS1Output_h

#include <Arduino.h>

/*
  TSS1 / R Heave 1 / R Heave 2 Data Format Output
  
  Format: 27-byte fixed length, 7-bit ASCII
  Message: : XX AAAA S MHHHH Q MRRRR S MPPPP <cr><lf>
  
  Fields:
  - `:` 	Start character
  - `XX`	Horizontal acceleration (Local X-Y). units: 0.0383m/s^2. ASCII HEX MSB first. Unsigned.
  - `AAAA`	Vertical acceleration (Local Z). units: 0.000625 m/s^2. ASCII HEX MSB first. (-20.48 to 20.48, Down: negative)
  - `S` 	Space 
  - `MHHHH` Heave. M = sign (space for +, '-' for -). HHHH = units of 0.01m (0000 to 9999). Above datum: Positive.
  - `Q` 	Status flag ('U' for unsettled/settled etc, we use 'U' for stable, 'u' for unstable)
  - `MRRRR` Roll. M = sign (space for +, '-' for -). RRRR = units of 0.01 deg. Port up: positive.
  - `S` 	Space
  - `MPPPP` Pitch. M = sign (space for +, '-' for -). PPPP = units of 0.01 deg. Bow up: positive.
  - `<cr><lf>` Terminator pair
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

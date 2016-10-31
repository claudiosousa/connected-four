#ifdef CAP_MRP121

// The default I2C address
#define MPR121_I2CADDR_DEFAULT 0x5A

#define MPR121_TOUCHSTATUS_L 0x00
#define MPR121_TOUCHSTATUS_H 0x01
#define MPR121_FILTDATA_0L 0x04
#define MPR121_FILTDATA_0H 0x05
#define MPR121_BASELINE_0 0x1E
#define MPR121_MHDR 0x2B
#define MPR121_NHDR 0x2C
#define MPR121_NCLR 0x2D
#define MPR121_FDLR 0x2E
#define MPR121_MHDF 0x2F
#define MPR121_NHDF 0x30
#define MPR121_NCLF 0x31
#define MPR121_FDLF 0x32
#define MPR121_NHDT 0x33
#define MPR121_NCLT 0x34
#define MPR121_FDLT 0x35

#define MPR121_TOUCHTH_0 0x41
#define MPR121_RELEASETH_0 0x42
#define MPR121_DEBOUNCE 0x5B
#define MPR121_CONFIG1 0x5C
#define MPR121_CONFIG2 0x5D
#define MPR121_CHARGECURR_0 0x5F
#define MPR121_CHARGETIME_1 0x6C
#define MPR121_ECR 0x5E
#define MPR121_AUTOCONFIG0 0x7B
#define MPR121_AUTOCONFIG1 0x7C
#define MPR121_UPLIMIT 0x7D
#define MPR121_LOWLIMIT 0x7E
#define MPR121_TARGETLIMIT 0x7F

#define MPR121_GPIODIR 0x76
#define MPR121_GPIOEN 0x77
#define MPR121_GPIOSET 0x78
#define MPR121_GPIOCLR 0x79
#define MPR121_GPIOTOGGLE 0x7A

#define MPR121_SOFTRESET 0x80

void setup_capacitive() {
  // soft reset
  writeRegister(MPR121_SOFTRESET, 0x63);

  delay(1);

  writeRegister(MPR121_ECR, 0x0);

  uint8_t c = readRegister8(MPR121_CONFIG2);

  if (c != 0x24) return;

  setThreshholds(12, 6);

  writeRegister(MPR121_MHDR, 0x01);
  writeRegister(MPR121_NHDR, 0x01);
  writeRegister(MPR121_NCLR, 0x0E);
  writeRegister(MPR121_FDLR, 0x00);

  writeRegister(MPR121_MHDF, 0x01);
  writeRegister(MPR121_NHDF, 0x05);
  writeRegister(MPR121_NCLF, 0x01);
  writeRegister(MPR121_FDLF, 0x00);

  writeRegister(MPR121_NHDT, 0x00);
  writeRegister(MPR121_NCLT, 0x00);
  writeRegister(MPR121_FDLT, 0x00);

  writeRegister(MPR121_DEBOUNCE, 0);
  writeRegister(MPR121_CONFIG1, 0x20);  // default, 16uA charge current
  writeRegister(MPR121_CONFIG2, 0x3A);  // 0.5uS encoding, 1ms period

  // enable all electrodes
  writeRegister(MPR121_ECR, 0x8F); // start with first 5 bits of baseline tracking
}

void setThreshholds(uint8_t touch, uint8_t release) {
  for (uint8_t i = 0; i < 12; i++) {
    writeRegister(MPR121_TOUCHTH_0 + 2 * i, touch);
    writeRegister(MPR121_RELEASETH_0 + 2 * i, release);
  }
}

int touchesref[7];
byte get_touches() {
  if (touchesref[0] == 0)
    read_touches(touchesref);
  byte res = 0;
  int touches[7];
  read_touches(touches);
  for (int i = 0; i < 7; i++) {
    if ((touchesref[i] - touches[i] >= 3))
      res |=  1 << i;
    touchesref[i] = (float)touchesref[i] * 0.8 + (float)touches[i] * 0.2;
  }
  return res;
}

void read_touches(int * res) {
  for (int i = 0; i < 7; i++)
    res[i] = readRegister16(MPR121_FILTDATA_0L + i * 2);
}

uint16_t get_touches__old(void) {

  uint16_t t = readRegister16(MPR121_TOUCHSTATUS_L);
  return t & 0x0FFF;
}


uint8_t readRegister8(uint8_t reg) {
  Wire.beginTransmission(MPR121_I2CADDR_DEFAULT);
  Wire.write(reg);
  Wire.endTransmission(false);
  while (Wire.requestFrom(MPR121_I2CADDR_DEFAULT, 1) != 1);
  return (Wire.read());
}

uint16_t readRegister16(uint8_t reg) {
  Wire.beginTransmission(MPR121_I2CADDR_DEFAULT);
  Wire.write(reg);
  Wire.endTransmission(false);
  while (Wire.requestFrom(MPR121_I2CADDR_DEFAULT, 2) != 2);
  uint16_t v = Wire.read();
  v |= ((uint16_t)Wire.read()) << 8;
  return v;
}

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPR121_I2CADDR_DEFAULT);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)(value));
  Wire.endTransmission();
}

#endif

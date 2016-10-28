// we don't initialize the CAP 1188 if we are using the MRP121
#ifndef CAP_MRP121

#define CAP1188_I2CADDR 0x29  // The default I2C address
#define CAP1188_SENINPUTSTATUS 0x3
#define CAP1188_MTBLK 0x2A
#define CAP1188_LEDLINK 0x72
#define CAP1188_STANDBYCFG 0x41
#define CAP1188_MAIN 0x00
#define CAP1188_MAIN_INT 0x01

void setup_capacitive() {
  writeRegister(CAP1188_MTBLK, 0);          // allow multiple touches
  writeRegister(CAP1188_LEDLINK, 0xFF);     // Have LEDs follow touches
  writeRegister(CAP1188_STANDBYCFG, 0x30);  // speed up a bit
}

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(CAP1188_I2CADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(CAP1188_I2CADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(CAP1188_I2CADDR, 1);
  return Wire.read();
}

uint8_t get_touches() {
  uint8_t t = readRegister(CAP1188_SENINPUTSTATUS);
  if (t)
    writeRegister(CAP1188_MAIN, readRegister(CAP1188_MAIN) & ~CAP1188_MAIN_INT);

  return t;
}
#endif

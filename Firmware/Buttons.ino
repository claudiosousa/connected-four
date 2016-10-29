#include <Wire.h>

const byte PIN_SCL = D2;
const byte PIN_SDA = D1;

const byte DEBOUCE_MS = 100;

void setup_buttons() {
  // Wire.begin(PIN_SDA, PIN_SCL);
  Wire.begin(PIN_SDA, PIN_SCL);
#ifdef DEBUG
// scanDevices();
#endif
  setup_capacitive();
}

void scanDevices() {
  byte error, address;
  int nb_devices = 0;

  Serial.println("Scanning I2C devices...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (!error) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");

      nb_devices++;
    } else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (!nb_devices) Serial.println("No I2C devices found\n");
}

unsigned long last_buttons_read = 0;
int last_touched_keys;
void loop_buttons() {
  if (millis() - last_buttons_read < DEBOUCE_MS) return;

  last_buttons_read = millis();

  int touched_keys = get_touches();
  if (!touched_keys || touched_keys == last_touched_keys) return;

  last_touched_keys = touched_keys;

  for (byte i = 0; i <= 7; i++) {
    String strbuttonnumber;
    if (touched_keys & (1 << i)) {
      publish_mqtt("button/" + device_id, String(i).c_str());
#ifdef DEBUG
      Serial.print("Button: ");
      Serial.println(i);
#endif
      return;
    }
  }
}

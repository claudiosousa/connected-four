/* Capacitive sensors CAP1188 management */

#include <Wire.h>

//PINS:
// SDA : GPIO05
// SCL : GPIO04

unsigned long buttonmillis;  // How long must a sensors stay active before to consider it
int buttonnumber = 0;        // wihch sensors is considered for
const int PIN_SCL = 4, PIN_SDA = 5;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup_buttons()
{
  Wire.begin(PIN_SDA, PIN_SCL);             //Initialise the I2C bus, PIN 4 = SCL, PIN 5 = SDA

  scanDevices();                //scan the devices connected to the I2C bus
  setup_cap1188();              //Setup the CAP1188
  buttonmillis = millis();      //initialise the variable with the actual millis (time) for delay.
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void scanDevices() {
  byte error, address;
  int nDevices;

#ifdef DEBUG
  Serial.println("Scanning I2C devices...");
#endif

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
#ifdef DEBUG
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
#endif

      nDevices++;
    }
    else if (error == 4)
    {
#ifdef DEBUG
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
#endif
    }
  }
#ifdef DEBUG
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop_buttons()     //"buttons" manager with capacitive sensors. This part will be improved
{
  int touchedKeys = cap1188_touches();          //State of the sensors
  String strbuttonnumber;                       // help to convert string to number

  if ( millis() - buttonmillis >= 150) {        //Look only x milliseconds the state of the capacitives sensors
//#ifdef DEBUGI2Cprint
//    Serial.print("b");
//#endif
    buttonmillis = millis();
    if (touchedKeys == 0) {
      // No touch detected
      buttonnumber = 0;
    }
    else {
      for (uint8_t i = 0; i < 8; i++) {
        if (touchedKeys & (1 << i) && i+1 != buttonnumber) {
          buttonnumber = i+1;
          strbuttonnumber = String(buttonnumber - 1);
          publish_mqtt("button/" + deviceId, strbuttonnumber.c_str());

#ifdef DEBUGI2Cprint
          Serial.print("C"); Serial.println(i + 1);
#endif

        }
      }
    }
  }

}

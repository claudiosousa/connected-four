/* Manage the OTA protocol, "Over The Air programming". This eneale the possibility to program the ESP via the LAN  */

#include <ArduinoOTA.h>

void setup_OTA() {
  ArduinoOTA.begin();
}

void loop_OTA() {
  ArduinoOTA.handle();
}


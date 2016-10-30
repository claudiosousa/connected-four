// Connect to the wifi using Wifimanager.h

/* This enable the possibility to connect the game on every wifi hot spot we want.
 *  
 * 
 */


#include <ESP8266WiFi.h>
#include <WiFiManager.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup_wifi() {

  delay(10);
#ifdef DEBUG
  Serial.println();
  Serial.print("Connecting...");
#endif

  WiFiManager wifiManager;
  //reset saved settings;
  //wifiManager.resetSettings();

  digitalWrite(INFO_LED, HIGH);
  wifiManager.autoConnect(("Connect4-" + deviceId).c_str());

  digitalWrite(INFO_LED, LOW);

#ifdef DEBUG
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(String(WiFi.localIP()));
#endif
}

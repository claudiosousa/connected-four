#include <ESP8266WiFi.h>   
#include <WiFiManager.h>

void setup_wifi() {
  delay(10);
#ifdef DEBUG
  Serial.println();
  Serial.print("Connecting...");
#endif

  WiFiManager wifiManager;
  // wifiManager.resetSettings();//reset saved settings;

  wifiManager.autoConnect(WIFI_HOSTNAME);

#ifdef DEBUG
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(String(WiFi.localIP()));
#endif
}

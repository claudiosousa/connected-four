#define DEBUG
//#define WS_79_LEDS //if using a 79 led strip
#define CAP_MRP121 //if using the capacitiv IC MRP121, instead of CAP1188

const String device_id = String(ESP.getChipId(), HEX); // Get the ID of the ESP8266 device

void setup() {       
#ifdef DEBUG
  Serial.begin(115200);
#endif

  setup_sound();
  setup_ws();           
  setup_buttons();      
  setup_wifi();          
  setup_OTA();          
  setup_mqtt();
}

void loop() {    
  loop_OTA();                   
  loop_mqtt();                
  loop_buttons();               
  loop_user();                  
  loop_ws();                    
  loop_sound();                 
}



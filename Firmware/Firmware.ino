#define DEBUG

const String device_id = String(ESP.getChipId(), HEX); // Get the ID of the ESP8266 device

void setup() {       
#ifdef DEBUG
  Serial.begin(115200);
#endif

  setup_ws();           
  setup_sound();       

  setup_buttons();      
  setup_wifi();         
  setup_user();         
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



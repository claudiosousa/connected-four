#define DEBUG             //For debuging process
//#define DEBUGI2Cprint     //For debuging process
//#define SendACKMsg        //Send back the receiving MQTT messages

const int dispdim = 2; // 1 = display with 43 leds, 2 = display with 79 leds.


boolean userstatus;             // Is user's turn. Last WS2812b led blink when = 1, 0 = stay illuminated
boolean startgame = true;       // To play start sound at start
int winloststatus = -1;           // -1 = play game, 0 = lost game, 1 = win game

const String deviceId = String(ESP.getChipId(), HEX); // Get the ID of the ESP8266 device

// parameters for pins I/O
int Nbr_LEDS;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {            //All the setup procedures

#ifdef DEBUG
  Serial.begin(115200);
#endif

  //setup_ws();           //WS2812b
  setup_sound();        //Sound

  //setup_buttons();      //CAP1188, capacitive sensors
  //setup_wifi();         //Wifi
  //setup_user();         //Player status
  //setup_OTA();          //OTA
  //setup_mqtt();         //MQTT

  playswinsound();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {     //Main loops procedures


  //loop_OTA();                   //OTA
 // loop_mqtt();                  //MQTT
  //loop_buttons();               //CAP1188
  //loop_user();                  //Player status
  //loop_ws();                    //WS2812b, leds
  loop_sound();                 //Sound

}



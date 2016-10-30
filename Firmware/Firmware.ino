/*
 *
 *  Connected4 - puissance 4 électronique
 *
 *  Electronique game developped by Claudio Sousa and Pierre-Frédéric Flammier
 *
 *  HEPIA's mini-project, August - september 2016
 *
 *  */



//#define DEBUG             //For debuging process
//#define DEBUGI2Cprint     //For debuging process
//#define SendACKMsg        //Send back the receiving MQTT messages

const int dispdim = 2; // 1 = display with 43 leds, 2 = display with 79 leds.


//Parameters for MQTT server (https://www.cloudmqtt.com)
const char* mqtt_server = "m20.cloudmqtt.com";
const char* mqtt_user = "esp";
const char* mqtt_password = "esp";
const int mqtt_port = 16384;

boolean userstatus;             // Is user's turn. Last WS2812b led blink when = 1, 0 = stay illuminated
boolean playsound = false;      // Play a sound
boolean startgame = true;       // To play start sound at start
int winloststatus = -1;           // -1 = play game, 0 = lost game, 1 = win game

const String deviceId = String(ESP.getChipId(), HEX); // Get the ID of the ESP8266 device

// parameters for pins I/O
const int INFO_LED      = 13;
const int PIN_WS        = 2;                    //Not used by the methode "Neo800KbpsMethod". This methode use the RX0 pin (GIPO03)
const int PINSOUNDON   = 12;
int Nbr_LEDS;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {            //All the setup procedures

#ifdef DEBUG
  Serial.begin(115200);
#endif

  pinMode(INFO_LED, OUTPUT);               // Initialize the pin INFO_LED as an output
  pinMode(PINSOUNDON, OUTPUT);             // Set PINSOUNDON to digital output
  digitalWrite(PINSOUNDON, 0);             // Set PINSOUNDON to cut the sound output


  setup_ws();           //WS2812b
  setup_sound();        //Sound

  setup_buttons();      //CAP1188, capacitive sensors
  setup_wifi();         //Wifi
  setup_user();         //Player status
  setup_OTA();          //OTA
  setup_mqtt();         //MQTT




}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {     //Main loops procedures


  loop_OTA();                   //OTA
  loop_mqtt();                  //MQTT
  loop_buttons();               //CAP1188
  loop_user();                  //Player status
  loop_ws();                    //WS2812b, leds
  loop_sound();                 //Sound

}



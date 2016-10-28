//MQTT manager

#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

long lastReconnectAttempt = 0;

boolean flashmqtt = false;
unsigned long flashmqttmillis = millis(), durationflash;
int cptflashmqtt, numflashmqtt;

const int numflashsendmqtt = 1;
const int numflashreceivemqtt = 3;
const unsigned long durationflashsendmqtt = 200, durationflashreceivemqtt = 67;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void flashsend() {            //Enable to blink the LED "INFO_LED" when sending a MQTT message
  flashmqtt = true;
  cptflashmqtt = 0;
  numflashmqtt = numflashsendmqtt;
  durationflash = durationflashsendmqtt;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void flashreceive() {         //Enable to blink the LED "INFO_LED" when receiving a MQTT message
  flashmqtt = true;
  cptflashmqtt = 0;
  numflashmqtt = numflashreceivemqtt;
  durationflash = durationflashreceivemqtt;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup_mqtt() {               // Setup the MQTT protocol
  espClient.setNoDelay(false);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqtt_callback);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop_mqtt() {

  ////////////////////////////
  if (!client.connected()) {
    long now = millis();

    if (lastReconnectAttempt == 0 || now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;

      // Attempt to reconnect
      if (mqtt_reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected
    client.loop();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void publish_mqtt(String topic, const char * msg) {     //Send a MQTT message with a String Topic (convert String topic on a topic pointer)
  publish_mqtt(topic.c_str(), msg);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void publish_mqtt(const char * topic, const char * msg) {   //Send a MQTT message with a topic pointer
  if (msg == NULL)
    msg = "";
  client.publish(topic, msg);

  flashsend();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool mqtt_reconnect() {               //Connect to MQTT server and subscribe to the neded topics
#ifdef DEBUG
  Serial.print("Attempting MQTT connection...");
#endif
  if (client.connect(device_id.c_str(), mqtt_user, mqtt_password)) {
#ifdef DEBUG
    Serial.print("connected deviceID = ");
    Serial.println(device_id);
#endif
    client.subscribe((device_id + "/#").c_str());
    client.subscribe("ping");

    publish_mqtt("connected/" + device_id, "");
  }
  return client.connected();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mqtt_callback(char* _topic, byte* payload, unsigned int length) {        //Receive all the suscribed MQTT messages
  flashreceive();
#ifdef DEBUG
  Serial.println("Receiving a message !");
#endif
  char msgArray[length + 1];
  for (unsigned int i = 0; i < length; i++) {
#ifdef DEBUG
    Serial.print((char)payload[i]);
#endif
    msgArray[i] = (char)payload[i];
  }
#ifdef DEBUG
  Serial.println();
#endif
  msgArray[length] = '\0';
  String msg = String(msgArray) ;
  String topic = String(_topic);

#ifdef DEBUG
  Serial.print("Receiving message. Topic : ");
  Serial.print(topic);
  Serial.print(", message : ");
  Serial.println(msg);
#endif

  /////////////////////////////////////////////////////////////////////// PING
  if (topic == String("ping")) {
#ifdef DEBUG
    Serial.println("Message is a ping request.");
#endif
    publish_mqtt("pong/" + device_id, "");
    return;
  }

  /////////////////////////////////////////////////////////////////////// GAME FINISHED
  if (topic == String(device_id + "/gamefinished")) {
#ifdef DEBUG
    Serial.println("Message is a game finished.");
#endif
    gamefinished_ws(msg);
    return;
  }

  /////////////////////////////////////////////////////////////////////// Go to Close add piece
  if (newpiece == true)
    closeaddpiece_ws();

  /////////////////////////////////////////////////////////////////////// ADD PIECE
  if ( (topic == String(device_id + "/addpiece")) /*&& (newpiece == false)*/ ) {
#ifdef DEBUG
    Serial.println("Message is for adding a piece.");
#endif
    addpiece_ws(msg);
    return;
  }

  /////////////////////////////////////////////////////////////////////// DEVICE COLOR
  if (topic == String(device_id + "/devicecolor")) {
#ifdef DEBUG
    Serial.println("Message is for setting user's color ");
#endif
    devicecolorsetting(msg);
    return;
  }

  /////////////////////////////////////////////////////////////////////// USER STATUS
  if (topic == String(device_id + "/user_turn")) {
#ifdef DEBUG
    Serial.println("Message is for setting user's status ");
#endif
    user_turnsetting(msg);
    return;
  }

  /////////////////////////////////////////////////////////////////////// SET BOARD
  if ( topic == String(device_id + "/setboard") ) {
#ifdef DEBUG
    Serial.println("Message is for setting the board ");
#endif
    boardsetting(msg);
    return;
  }

  /////////////////////////////////////////////////////////////////////// MATRIX
  if (topic == String(device_id + "/matrix")) {
#ifdef DEBUG
    Serial.println("Message is for setting the board (matrix).");
#endif
    matrix_ws(msg);
    return;
  }

  /////////////////////////////////////////////////////////////////////// SET ROW
  if (topic == String(device_id + "/setrow")) {
#ifdef DEBUG
    Serial.println("Message is for setting a row (matrix).");
#endif
    row_ws(msg);
    return;
  }

  /////////////////////////////////////////////////////////////////////// SET VOLUME LEVEL
  if (topic == String(device_id + "/volume")) {
#ifdef DEBUG
    Serial.println("Message is for setting the sound volume.");
#endif
    setvolume(msg);
    return;
  }
  /////////////////////////////////////////////////////////////////////// Test mqtt messages
  if (topic == String(device_id + "/test")) {
#ifdef DEBUG
    Serial.println("Message is a test message.");
#endif
    publish_mqtt("testedmessage/" + device_id, msg.c_str());
    return;
  }

}


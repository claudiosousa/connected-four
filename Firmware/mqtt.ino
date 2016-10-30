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
    digitalWrite(INFO_LED, LOW);
    client.loop();
  }

  ////////////////////////////// Blink the LED "INFO_LED"

  if (flashmqtt && (millis() - flashmqttmillis >= durationflash) ) {
    flashmqttmillis = millis();
    if (cptflashmqtt % 2 == 0)
      digitalWrite(INFO_LED, 1);
    else
      digitalWrite(INFO_LED, 0);
    cptflashmqtt++;
    if (cptflashmqtt > numflashmqtt)
      flashmqtt = false;
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
  if (client.connect(deviceId.c_str(), mqtt_user, mqtt_password)) {
#ifdef DEBUG
    Serial.print("connected deviceID = ");
    Serial.println(deviceId);
#endif
    client.subscribe((deviceId + "/#").c_str());
    client.subscribe("ping");

    publish_mqtt("connected/" + deviceId, "");
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
    publish_mqtt("pong/" + deviceId, "");
    return;
  }

  /////////////////////////////////////////////////////////////////////// GAME FINISHED
  if (topic == String(deviceId + "/gamefinished")) {
#ifdef DEBUG
    Serial.println("Message is a game finished.");
#endif
    gamefinished_ws(msg);
#ifdef SendACKMsg
    publish_mqtt("gamefinishedupdated/" + deviceId, msg.c_str());
#endif
    return;
  }

  /////////////////////////////////////////////////////////////////////// Go to Close add piece
  if (newpiece == true)
    closeaddpiece_ws();

  /////////////////////////////////////////////////////////////////////// ADD PIECE
  if ( (topic == String(deviceId + "/addpiece")) /*&& (newpiece == false)*/ ) {
#ifdef DEBUG
    Serial.println("Message is for adding a piece.");
#endif
    addpiece_ws(msg);
#ifdef SendACKMsg
    publish_mqtt("pieceadded/" + deviceId, msg.c_str());
#endif
    return;
  }

  /////////////////////////////////////////////////////////////////////// DEVICE COLOR
  if (topic == String(deviceId + "/devicecolor")) {
#ifdef DEBUG
    Serial.println("Message is for setting user's color ");
#endif
    devicecolorsetting(msg);
#ifdef SendACKMsg
    publish_mqtt("devicecolorupdated/" + deviceId, msg.c_str());
#endif
    return;
  }

  /////////////////////////////////////////////////////////////////////// USER STATUS
  if (topic == String(deviceId + "/userstatus")) {
#ifdef DEBUG
    Serial.println("Message is for setting user's status ");
#endif
    userstatussetting(msg);
#ifdef SendACKMsg
    publish_mqtt("userstatusupdated/" + deviceId, msg.c_str());
#endif
    return;
  }

  /////////////////////////////////////////////////////////////////////// SET BOARD
  if ( topic == String(deviceId + "/setboard") ) {
#ifdef DEBUG
    Serial.println("Message is for setting the board ");
#endif
    boardsetting(msg);
#ifdef SendACKMsg
    publish_mqtt("setboardupdated/" + deviceId, msg.c_str());
#endif
    return;
  }

  /////////////////////////////////////////////////////////////////////// MATRIX
  if (topic == String(deviceId + "/matrix")) {
#ifdef DEBUG
    Serial.println("Message is for setting the board (matrix).");
#endif
    matrix_ws(msg);
#ifdef SendACKMsg
    publish_mqtt("matrixupdated/" + deviceId, msg.c_str());
#endif
    return;
  }

  /////////////////////////////////////////////////////////////////////// SET ROW
  if (topic == String(deviceId + "/setrow")) {
#ifdef DEBUG
    Serial.println("Message is for setting a row (matrix).");
#endif
    row_ws(msg);
#ifdef SendACKMsg
    publish_mqtt("rowupdated/" + deviceId, msg.c_str());
#endif
    return;
  }

  /////////////////////////////////////////////////////////////////////// SET VOLUME LEVEL
  if (topic == String(deviceId + "/volume")) {
#ifdef DEBUG
    Serial.println("Message is for setting the sound volume.");
#endif
    setvolume(msg);
#ifdef SendACKMsg
    publish_mqtt("volumeupdated/" + deviceId, msg.c_str());
#endif
    return;
  }
  /////////////////////////////////////////////////////////////////////// Test mqtt messages
  if (topic == String(deviceId + "/test")) {
#ifdef DEBUG
    Serial.println("Message is a test message.");
#endif
    publish_mqtt("testedmessage/" + deviceId, msg.c_str());
    return;
  }

}


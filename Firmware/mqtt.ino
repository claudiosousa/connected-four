#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

long lastReconnectAttempt = 0;

void setup_mqtt() {
  espClient.setNoDelay(false);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqtt_callback);
}

bool mqtt_reconnect() {
#ifdef DEBUG
  Serial.print("Attempting MQTT connection...");
#endif

  if (client.connect(device_id.c_str(), mqtt_user, mqtt_password)) {
    client.subscribe((device_id + "/#").c_str());
    client.subscribe("ping");

    publish_mqtt("connected/" + device_id, "");
  }
  return client.connected();
}

void loop_mqtt() {
  if (!client.connected()) {
    long now = millis();

    if (lastReconnectAttempt == 0 || now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;

      // Attempt to reconnect
      if (mqtt_reconnect()) 
        lastReconnectAttempt = 0;
    }
  } else 
    client.loop();
}

void publish_mqtt(String topic, const char* msg) {
  publish_mqtt(topic.c_str(), msg);
}

void publish_mqtt(const char* topic, const char* msg) {
  if (msg == NULL)
    msg = "";
  client.publish(topic, msg);
}

void mqtt_callback(char* _topic, byte* payload, unsigned int length) {
#ifdef DEBUG
  Serial.println("Receiving a message !");
#endif

  char msgArray[length + 1];
  memcpy(msgArray, payload, length);
  msgArray[length] = 0;
  String topic = String(_topic);

  if (topic == String("ping"))
    return publish_mqtt("pong/" + device_id, NULL);
  if (topic == String(device_id + "/volume"))
    return setvolume(String(msgArray).toInt());
  if (topic == String(device_id + "/gamefinished"))
    return set_game_finished(msgArray);
  if (topic == String(device_id + "/addpiece"))
    return addpiece(msgArray);
  if (topic == String(device_id + "/devicecolor"))
    return setdevicecolor(msgArray);
  if (topic == String(device_id + "/setboard"))
    return set_board_pieces(msgArray);
}


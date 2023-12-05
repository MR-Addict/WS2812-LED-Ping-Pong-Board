void handleMQTTMessage(String &topic, String &payload) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);

  if (doc.containsKey("state")) isDisplay = (bool)(doc["state"] == "on");
  if (doc.containsKey("mode")) {
    isDisplay = true;
    Mode = (uint8_t)doc["mode"];
  }
  if (doc.containsKey("hour") && Mode == 0 && isDisplay) {
    HOUR = (uint8_t)doc["hour"];
    MINUTE = (uint8_t)doc["minute"];
    SetTime();
  }
  if (doc.containsKey("year") && Mode == 1 && isDisplay) {
    YEAR = (uint16_t)doc["year"];
    MONTH = (uint8_t)doc["month"];
    DATE = (uint8_t)doc["date"];
    SetTime();
  }
  publishMessage();
}

void mqttConnect() {
  mqtt.setKeepAlive(60);
  mqtt.setCleanSession(false);
  mqtt.setWill(mqtt_will_topic, mqtt_will_msg, true, 0);

  uint8_t attempts = 0, maxAttempts = 10;
  while (!mqtt.connected() && attempts < maxAttempts) {
    if (mqtt.connect(mqtt_id, mqtt_user, mqtt_password)) {
      Serial.printf("MQTT broker: %s\n", mqtt_broker);
    } else {
      delay(1000);
      Serial.println("Failed to connect to MQTT broker, retrying in 5 seconds...");
    }
    attempts++;
  }

  if (attempts == 10) {
    mqttBrokerFailed = true;
    Serial.println("Max attemptes, abort mqtt API");
  }
  else mqtt.subscribe(mqtt_sub_topic);
}

void mqttSetup() {
  mqtt.begin(mqtt_broker, 1883, espClient);
  mqtt.onMessage(handleMQTTMessage);
  mqttConnect();
  publishMessage();
}

void mqttLoop() {
  if (!mqttBrokerFailed) {
    mqtt.loop();
    if (!mqtt.connected())mqttConnect();
  }
}

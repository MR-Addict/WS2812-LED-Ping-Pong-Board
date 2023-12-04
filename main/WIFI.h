void connectionLostCallback(WiFiEvent_t event, WiFiEventInfo_t info) {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("WiFi connection lost. Try to reconnect...");
}

void setupWiFi() {
  // Init SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname);

  // Set WIFI disconnect event
  WiFi.onEvent(connectionLostCallback, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  // Set station mode
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Connecting to %s", WIFI_SSID);

  // Connect to WIFI
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.printf("\nLocal IP: %s\n", WiFi.localIP().toString().c_str());
}

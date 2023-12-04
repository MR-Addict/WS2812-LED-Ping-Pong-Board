void handleWebSocketMessage(const uint32_t client_id, uint8_t *payload, size_t len) {
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payload, len);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

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

void webSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* payload, size_t len) {
  const uint32_t client_id = client->id();
  const String client_ip = client->remoteIP().toString();

  switch (type) {
    case WS_EVT_DATA: handleWebSocketMessage(client_id, payload, len); break;
    case WS_EVT_CONNECT: publishMessage(); break;
    case WS_EVT_DISCONNECT: break;
    case WS_EVT_ERROR: break;
    case WS_EVT_PONG: break;
  }
}

// handle OPTIONS and 404 request
void handleNotFound(AsyncWebServerRequest* request) {
  String url = request->url();

  // end with /
  if (url.endsWith("/")) {
    url += "index.html";
    if (SPIFFS.exists(url)) request-> send(SPIFFS, url, "text/html");
    else request->send(404);
  }
  else if (request->method() == HTTP_OPTIONS) request->send(200);
  else request->send(404, "text/plain", "404");
}

void setupServer() {
  websocket.onEvent(webSocketEvent);
  server.addHandler(&websocket);

  server.serveStatic("/", SPIFFS, "/");
  server.onNotFound(handleNotFound);
  server.begin();
}

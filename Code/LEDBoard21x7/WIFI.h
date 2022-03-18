void webSocketEvent(uint8_t num,
                    WStype_t type,
                    uint8_t* payload,
                    size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
                IPAddress ip = websocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num,
                              ip[0], ip[1], ip[2], ip[3], payload);

                String message = "{\"LED\":\"OFF\",\"Mode\":";
                if (isDisplay)
                    message = "{\"LED\":\"ON\",\"Mode\":";
                message = message + Mode + ",\"Year\":" + YEAR +
                          ",\"Month\":" + MONTH + ",\"Date\":" + DATE +
                          ",\"Hour\":" + HOUR + ",\"Minute\":" + MINUTE + '}';

                websocket.sendTXT(num, message);
            } break;
        case WStype_TEXT: {
                Serial.printf("[%u] get Text: %s\n", num, payload);
                String message = String((char*)(payload));
                DynamicJsonDocument doc(100);
                DeserializationError error = deserializeJson(doc, message);
                if (error) {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.f_str());
                    return;
                }
                if (doc.containsKey("LED")) {
                    if (doc["LED"] == "ON")
                        isDisplay = true;
                    else if (doc["LED"] == "OFF")
                        isDisplay = false;
                } else if (doc.containsKey("Mode")) {
                    Mode = (uint8_t)doc["Mode"];
                } else if (doc.containsKey("Hour") && Mode == 0) {
                    HOUR = (uint8_t)doc["Hour"];
                    MINUTE = (uint8_t)doc["Minute"];
                    SetTime();
                } else if (doc.containsKey("Year") && Mode == 1) {
                    YEAR = (uint16_t)doc["Year"];
                    MONTH = (uint8_t)doc["Month"];
                    DATE = (uint8_t)doc["Date"];
                    SetTime();
                }
                websocket.broadcastTXT(message);
            } break;
    }
}

void notFound(AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Page Not found");
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    // Reconnect
    Serial.print("WiFi lost connection. Reason: ");
    Serial.println(info.disconnected.reason);
    Serial.println("Trying to Reconnect");
    WiFi.begin(ssid, password);
}

void WIFI_Init() {
    // Init SPIFFS
    if (!SPIFFS.begin()) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    // change hostname to ledboard-507
    String hostname = "ledboard-507";
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(hostname.c_str());

    // Set WIFI disconnect event
    WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);

    // Set station mode
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.print(ssid);

    // Connect to WIFI
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print('.');
    }
    Serial.println();
    Serial.println(WiFi.localIP());
}

void Server_Init() {
    // Home page
    server.on("/", [](AsyncWebServerRequest * request) {
        // Get POST data
        String user = "", pwd = "";
        uint8_t params = request->params();
        for (uint8_t i = 0; i < params; i++) {
            AsyncWebParameter* p = request->getParam(i);
            if (strcmp(p->name().c_str(), "username") == 0) {
                user = p->value().c_str();
            } else if (strcmp(p->name().c_str(), "password") == 0) {
                pwd = p->value().c_str();
            }
        }

        // check user and pwd
        if (user == login_user && pwd == login_pwd) {
            request->send(SPIFFS, "/index.html", "text/html");
        } else {
            request->redirect("/login");
        }
    });

    // Login page
    server.on("/login", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(SPIFFS, "/login.html", "text/html");
    });

    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(SPIFFS, "/favicon.png", "image/png");
    });

    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(SPIFFS, "/style.css", "text/css");
    });

    server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(SPIFFS, "/index.js", "text/js");
    });

    server.onNotFound(notFound);
    server.begin();
    websocket.begin();
    websocket.onEvent(webSocketEvent);
}

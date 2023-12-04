// Header files for BT
#include <RTClib.h>
#include <FastLED.h>

// Header files for WiFi
#include <WiFi.h>
#include <MQTT.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

// Header files for custom functions
#include "header.h"
#include "wifi.h"
#include "lamp.h"
#include "mqtt.h"
#include "server.h"

void setup() {
  Serial.begin(115200);
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  pinMode(Button3, INPUT_PULLUP);
  pinMode(Button4, INPUT_PULLUP);

  // 启动DS3231
  rtc.begin();
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), alarm_irq, FALLING);
  rtc.setAlarm1(DateTime(2021, 12, 28, 0, 0, 0), DS3231_A1_Hour); // 每日凌晨0点关闭LED
  rtc.setAlarm2(DateTime(2021, 12, 28, 12, 0, 0), DS3231_A2_Hour); // 每日中午12点打开LED
  DateTime now = rtc.now();
  YEAR = now.year(), MONTH = now.month(), DATE = now.day(), HOUR = now.hour(), MINUTE = now.minute();

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();

  setupWiFi();
  mqttSetup();
  setupServer();
}

void loop() {
  mqttLoop();
  checkAlarm();
  checkButton();
  websocket.cleanupClients();
  EVERY_N_MILLISECONDS(20) Display();
}

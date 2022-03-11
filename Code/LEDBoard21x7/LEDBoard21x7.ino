// Header files for BT
#include <BluetoothSerial.h>
#include <FastLED.h>
#include <RTClib.h>

// Header files for WIFI
#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

// Header files for custom functions
#include "arduino_secrets.h"
#include "HeaderConfig.h"
#include "LEDFunction.h"
#include "WIFI.h"

void setup() {
    Serial.begin(115200);
    pinMode(Button1, INPUT_PULLUP);
    pinMode(Button2, INPUT_PULLUP);
    pinMode(Button3, INPUT_PULLUP);
    pinMode(Button4, INPUT_PULLUP);

    // 设置蓝牙名称
    SerialBT.begin("LED Board 507");

    // 启动DS3231
    rtc.begin();
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
    attachInterrupt(digitalPinToInterrupt(INT_PIN), alarm_irq, FALLING);
    rtc.setAlarm1(DateTime(2021, 12, 28, 0, 0, 0), DS3231_A1_Hour); // 每日凌晨12点关闭LED
    rtc.setAlarm2(DateTime(2021, 12, 28, 8, 0, 0), DS3231_A2_Hour); // 每日早晨8点打开LED
    DateTime now = rtc.now();
    YEAR = now.year(), MONTH = now.month(), DATE = now.day(), HOUR = now.hour(),
    MINUTE = now.minute();

    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();

    WIFI_Init();
    Server_Init();
}

void loop() {
    checkAlarm();
    Get_Button();
    Get_Serial();
    websocket.loop();
    EVERY_N_MILLISECONDS(20) Display();
}

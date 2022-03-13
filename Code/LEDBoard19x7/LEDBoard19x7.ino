#include <BluetoothSerial.h>
#include <FastLED.h>
#include <RTClib.h>

#include "HeaderConfig.h"
#include "LEDFunction.h"

void setup() {
    pinMode(Button1, INPUT_PULLUP);
    pinMode(Button2, INPUT_PULLUP);
    pinMode(Button3, INPUT_PULLUP);
    pinMode(Button4, INPUT_PULLUP);

    // 设置蓝牙名称
    SerialBT.begin("LED Board 508");
    rtc.begin();
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
    attachInterrupt(digitalPinToInterrupt(INT_PIN), alarm_irq, FALLING);
    rtc.setAlarm1(DateTime(2021, 12, 28, 0, 0, 0), DS3231_A1_Hour); // 每日凌晨12点关闭LED
    rtc.setAlarm2(DateTime(2021, 12, 28, 12, 0, 0), DS3231_A2_Hour); // 每日早晨12点打开LED
    if (rtc.lostPower())
        rtc.adjust(DateTime(2021, 10, 20, 12, 30, 0));
    DateTime now = rtc.now();
    YEAR = now.year(), MONTH = now.month(), DATE = now.day(), HOUR = now.hour(),
    MINUTE = now.minute();

    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS)
        .setCorrection(TypicalSMD5050);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
}

void loop() {
    checkAlarm();
    Get_Button();
    Get_Serial();
    EVERY_N_MILLISECONDS(20) Display();
}

#include <BluetoothSerial.h>
#include <RTClib.h>
#include <FastLED.h>

#include "HeaderConfig.h"
#include "LEDFunction.h"
#include "ControlFunction.h"

void setup() {
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  pinMode(Button3, INPUT_PULLUP);
  pinMode(Button4, INPUT_PULLUP);

  SerialBT.begin("LED Board");
  rtc.begin();
  if (rtc.lostPower())rtc.adjust(DateTime(2021, 10, 20, 12, 30, 0));
  DateTime now = rtc.now();
  YEAR = now.year(), MONTH = now.month(), DATE = now.day(), HOUR = now.hour(), MINUTE = now.minute();

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.clear();

}

void loop() {
  Get_Button();
  Get_Serial();
  EVERY_N_MILLISECONDS(20)Display();
}

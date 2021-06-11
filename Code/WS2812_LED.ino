#include <FastLED.h>
#include <DS1302.h>

boolean ScrollScreen;
boolean Dispaly_Status;

uint8_t Delay = 20;
uint8_t Mode;
uint8_t Mode2;
uint8_t Mode3;
uint8_t BRIGHTNESS = 128;
uint8_t R = 255, G = 255, B = 0;
uint8_t Month, Date, Hour, Minute;
uint8_t twinkleChance = 255;
int Year;

const uint8_t LED_PIN = 2;
const uint8_t CLK = 3;
const uint8_t DAT = 4;
const uint8_t RST = 5;
const uint8_t NUM_LEDS = 135;
const uint8_t twinkleInterval = 100;

const uint8_t Number[10][13] = {
  {34, 35, 36, 38, 40, 73, 75, 80, 82, 112, 113, 114},
  {34, 35, 36, 39, 74, 81, 113, 114},
  {34, 35, 36, 38, 73, 74, 75, 82, 112, 113, 114},
  {34, 35, 36, 40, 73, 74, 75, 82, 112, 113, 114},
  {34, 40, 73, 74, 75, 80, 82, 112, 114},
  {34, 35, 36, 40, 73, 74, 75, 80, 112, 113, 114},
  {34, 35, 36, 40, 38, 73, 74, 75, 80, 112, 113, 114},
  {34, 40, 73, 82, 112, 113, 114},
  {34, 35, 36, 38, 40, 73, 74, 75, 80, 82, 112, 113, 114},
  {34, 35, 36, 40, 73, 74, 75, 80, 82, 112, 113, 114}
};

CRGBPalette16 myColorPalette = CRGBPalette16(
                                 CRGB::Red,  CRGB::Black,  CRGB::Green,  CRGB::Blue,
                                 CRGB::Red,  CRGB::Black,  CRGB::Green,  CRGB::Blue,
                                 CRGB::Red,  CRGB::Black,  CRGB::Green,  CRGB::Blue,
                                 CRGB::Red,  CRGB::Black,  CRGB::Green,  CRGB::Blue
                               );
CRGBPalette16 myColorPalette2 = CRGBPalette16(
                                  CRGB::Black,  CRGB::Black,  CRGB::Black,  CRGB::Black,
                                  CRGB::Black,  CRGB::Black,  CRGB::Black,  CRGB::Black,
                                  CRGB::Black,  CRGB::Black,  CRGB::Black,  CRGB::Black,
                                  CRGB::Red,  CRGB::Green,  CRGB::Blue,  CRGB::Black
                                );

CRGB leds[NUM_LEDS];
DS1302 rtc(RST, DAT, CLK);

void setup() {
  delay(3000);
  Serial.begin(9600);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  Time t = rtc.time();
  Year = t.yr;
  Month = t.mon;
  Date = t.date;
  Hour = t.hr;
  Minute = t.min;
}

void loop() {
  GetSerial();
  if (Dispaly_Status) {
    if (Mode == 0)Single_Color();
    else if (Mode == 1) {
      Dynamic_Color();
    }
    else if (Mode == 2) {
      if (ScrollScreen)Scroll_Time();
      else TimeMode();
    }
    else if (Mode == 3)UIMode();
  }
  else Turn_OFF();
  FastLED.show();
  delay(Delay);
}

//获取（蓝牙）串数据
void GetSerial() {
  if (Serial.available()) {
    uint8_t message = Serial.read();
    uint8_t parameter = Serial.parseInt();
    if (message == 'M' || message == 'm') {
      switch (parameter) {
        case 0: Mode = 0; break;
        case 1: Mode = 1; break;
        case 2: Mode = 2; break;
        case 3: Mode = 3; break;
      }
    }
    else if (message == 'A' || message == 'a') {
      if (Mode == 1) {
        switch (parameter) {
          case 0: Mode2 = 0; break;
          case 1: Mode2 = 1; Turn_OFF(); break;
          case 2: Mode2 = 2; break;
          case 3: Mode2 = 3; break;
          case 4: Mode2 = 4; break;
          case 5: Mode2 = 5; Turn_OFF(); break;
          case 6: Mode2 = 6; break;
        }
      }
    }
    else if (message == 'B' || message == 'b') {
      if (Mode == 2) {
        switch (parameter) {
          case 0: Mode3 = 0; break;
          case 1: Mode3 = 1; break;
          case 2: Mode3 = 2; break;
          case 3: Mode3 = 3; break;
          case 4: Mode3 = 4; break;
          case 5: Mode3 = 5; break;
          case 6: Mode3 = 6; break;
          case 7: ScrollScreen = !ScrollScreen; break;
        }
      }
    }
    else if (message == 'S' || message == 's') {
      switch (parameter) {
        case 0: Dispaly_Status = false; break;
        default: Dispaly_Status = true; break;
      }
    }
    else if (message == 'B' || message == 'b') {
      BRIGHTNESS = parameter;
      if (Mode == 0 || Mode == 2) {
        FastLED.setBrightness(BRIGHTNESS);
      }
    }
    else if (message == 'R' || message == 'r') {
      R = parameter;
      G = Serial.parseInt();
      B = Serial.parseInt();
    }
    else if (message == 'D' || message == 'd') {
      Delay = parameter;
    }
    else if (message == 'H' || message == 'h') {
      if (Mode == 2) {
        Hour = parameter;
        Minute = Serial.parseInt();
        SetTime();
      }
    }
  }
}

//关闭LED
void Turn_OFF() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}

//单色模式
void Single_Color() {
  fill_solid(leds, NUM_LEDS, CRGB(R, G, B));
}

//动态模式
void Dynamic_Color() {
  switch (Mode2) {
    case 0: Scan(); break;
    case 1: Pour(); break;
    case 2: Arrow(); break;
    case 3: Rainbow(); break;
    case 4: Faded(); break;
    case 5: Twinkle(); break;
    case 6: Random(); break;
  }
}

//动态模式——小星星
void Twinkle() {
  uint8_t pos = random8(NUM_LEDS);
  EVERY_N_MILLISECONDS(twinkleInterval) {
    if ( random8() < twinkleChance) {
      leds[pos] = ColorFromPalette( PartyColors_p, random8(255), 128, LINEARBLEND);
    }
  }
  EVERY_N_MILLISECONDS( 20 ) {
    fadeToBlackBy( leds, NUM_LEDS, 10);
  }
}

//动态模式——闪烁
void Faded() {
  static boolean flag;
  static uint8_t max_bright = 230;
  if (flag)max_bright += 10;
  else max_bright -= 10;
  if (max_bright == 30)flag = true;
  else if (max_bright == 230)flag = false;
  FastLED.setBrightness(max_bright);
  fill_solid(leds, NUM_LEDS, CRGB(R, G, B));
  FastLED.show();
  delay(Delay);
}

//动态模式——彩虹
void Rainbow() {
  static uint8_t count;
  static boolean flag;
  if (flag) {
    Fill_Rainbow(0, 18, count + 12, 4);
    Fill_Rainbow(37, 18, count + 8, 4);
    Fill_Rainbow(37, 57, count + 4, 4);
    Fill_Rainbow(78, 57, count, 4);
    Fill_Rainbow(78, 98, count, 4);
    Fill_Rainbow(117, 98, count, 4);
    Fill_Rainbow(117, 135, count, 4);
  }
  else {
    Fill_Rainbow(0, 18, count, 4);
    Fill_Rainbow(37, 18, count, 4);
    Fill_Rainbow(37, 57, count, 4);
    Fill_Rainbow(78, 57, count, 4);
    Fill_Rainbow(78, 98, count + 4, 4);
    Fill_Rainbow(117, 98, count + 8, 4);
    Fill_Rainbow(117, 135, count + 12, 4);
  }

  if (flag)count --;
  else count ++;
  if (count == 0)flag = false;
  else if (count == 200)flag = true;
}

void Fill_Rainbow(uint8_t start, uint8_t stop, uint8_t Color_Begin, uint8_t grid) {
  uint8_t index = Color_Begin;
  if (start < stop) {
    for (uint8_t i = start; i < stop; i++) {
      leds[i] = ColorFromPalette( myColorPalette, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
  else if (start > stop) {
    for (uint8_t i = start - 1; i >= stop; i--) {
      leds[i] = ColorFromPalette( myColorPalette, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
}


//动态模式——箭头
void Fill_Arrow(uint8_t start, uint8_t stop, uint8_t Color_Begin, uint8_t grid) {
  uint8_t index = Color_Begin;
  if (start < stop) {
    for (uint8_t i = start; i < stop; i++) {
      leds[i] = ColorFromPalette( myColorPalette2, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
  else if (start > stop) {
    for (uint8_t i = start - 1; i >= stop; i--) {
      leds[i] = ColorFromPalette( myColorPalette2, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
}

void Arrow() {
  static uint8_t count = 14;
  static boolean flag;
  if (flag) {
    Fill_Arrow(0, 18, count + 24, 8);
    Fill_Arrow(37, 18, count + 16, 8);
    Fill_Arrow(37, 57, count + 8, 8);
    Fill_Arrow(78, 57, count, 8);
    Fill_Arrow(78, 98, count + 8, 8);
    Fill_Arrow(117, 98, count + 16, 8);
    Fill_Arrow(117, 135, count + 24, 8);
  }
  else {
    Fill_Arrow(0, 18, count, 8);
    Fill_Arrow(37, 18, count, 8);
    Fill_Arrow(37, 57, count, 8);
    Fill_Arrow(78, 57, count, 8);
    Fill_Arrow(78, 98, count, 8);
    Fill_Arrow(117, 98, count, 8);
    Fill_Arrow(117, 135, count, 8);
  }
  if (flag)count -= 2;
  else count += 2;
  if (count == 14)flag = false;
  else if (count == 248)flag = true;
}

//动态模式——扫描
void Scan() {
  static boolean flag;
  static uint8_t LED_Index;
  static uint8_t Color_Index;
  leds[LED_Index] = ColorFromPalette( myColorPalette, Color_Index, BRIGHTNESS, LINEARBLEND);
  if (flag)LED_Index --;
  else LED_Index++;
  if (LED_Index == 0) {
    flag = false;
    Color_Index += 16;
  }
  if (LED_Index == 135) {
    flag = true;
    Color_Index += 16;
  }
}

//动态模式——瀑布
void Pour() {
  static boolean flag;
  static uint8_t LED_Index;
  static uint8_t Color_Index;
  static uint8_t count;
  CRGB COLOR = ColorFromPalette( myColorPalette, Color_Index, BRIGHTNESS, LINEARBLEND);
  switch (LED_Index) {
    case 0: fill_solid(leds, 18, COLOR); break;
    case 1: fill_solid(leds + 18, 19, COLOR); break;
    case 2: fill_solid(leds + 37, 20, COLOR); break;
    case 3: fill_solid(leds + 57, 21, COLOR); break;
    case 4: fill_solid(leds + 78, 20, COLOR); break;
    case 5: fill_solid(leds + 98, 19, COLOR); break;
    case 6: fill_solid(leds + 117, 18, COLOR); break;
  }
  count++;
  if (count == 15) {
    count = 0;
    if (flag)LED_Index --;
    else LED_Index++;
    if (LED_Index == 0) {
      flag = false;
      Color_Index += 16;
    }
    if (LED_Index == 6) {
      flag = true;
      Color_Index += 16;
    }
  }
}

//随机显示动态模式
void Random() {
  static uint16_t count;
  static uint8_t index = random8(6);
  switch (index) {
    case 0: Scan(); break;
    case 1: Pour(); break;
    case 2: Arrow(); break;
    case 3: Rainbow(); break;
    case 4: Twinkle(); break;
    case 5: Faded(); break;
  }
  count++;
  if (count == 500) {
    index = random8(6);
    count = 0;
  }
}

//时钟背景白云
void Cloud() {
  static uint8_t count;
  Fill_Cloud(0, 18, count, 5);
  Fill_Cloud(37, 18, count, 5);
  Fill_Cloud(37, 57, count, 5);
  Fill_Cloud(78, 57, count, 5);
  Fill_Cloud(78, 98, count + 5, 5);
  Fill_Cloud(117, 98, count + 10, 5);
  Fill_Cloud(117, 135, count + 15, 5);
  count ++;
}

void Fill_Cloud(uint8_t start, uint8_t stop, uint8_t Color_Begin, uint8_t grid) {
  uint8_t index = Color_Begin;
  if (start < stop) {
    for (uint8_t i = start; i < stop; i++) {
      leds[i] = ColorFromPalette( CloudColors_p, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
  else if (start > stop) {
    for (uint8_t i = start - 1; i >= stop; i--) {
      leds[i] = ColorFromPalette( CloudColors_p, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
}

//时钟背景——大海
void Ocean() {
  static uint8_t count;
  Fill_Ocean(0, 18, count, 5);
  Fill_Ocean(37, 18, count, 5);
  Fill_Ocean(37, 57, count, 5);
  Fill_Ocean(78, 57, count, 5);
  Fill_Ocean(78, 98, count + 5, 5);
  Fill_Ocean(117, 98, count + 10, 5);
  Fill_Ocean(117, 135, count + 15, 5);
  count ++;
}

void Fill_Ocean(uint8_t start, uint8_t stop, uint8_t Color_Begin, uint8_t grid) {
  uint8_t index = Color_Begin;
  if (start < stop) {
    for (uint8_t i = start; i < stop; i++) {
      leds[i] = ColorFromPalette( OceanColors_p, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
  else if (start > stop) {
    for (uint8_t i = start - 1; i >= stop; i--) {
      leds[i] = ColorFromPalette( OceanColors_p, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
}

//时钟背景——岩浆
void Heat() {
  static uint8_t count;
  Fill_Heat(0, 18, count, 5);
  Fill_Heat(37, 18, count, 5);
  Fill_Heat(37, 57, count, 5);
  Fill_Heat(78, 57, count, 5);
  Fill_Heat(78, 98, count + 5, 5);
  Fill_Heat(117, 98, count + 10, 5);
  Fill_Heat(117, 135, count + 15, 5);
  count ++;
}

void Fill_Heat(uint8_t start, uint8_t stop, uint8_t Color_Begin, uint8_t grid) {
  uint8_t index = Color_Begin;
  if (start < stop) {
    for (uint8_t i = start; i < stop; i++) {
      leds[i] = ColorFromPalette( LavaColors_p, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
  else if (start > stop) {
    for (uint8_t i = start - 1; i >= stop; i--) {
      leds[i] = ColorFromPalette( LavaColors_p, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
}

//时钟背景——森林
void Forest() {
  static uint8_t count;
  Fill_Forest(0, 18, count, 5);
  Fill_Forest(37, 18, count, 5);
  Fill_Forest(37, 57, count, 5);
  Fill_Forest(78, 57, count, 5);
  Fill_Forest(78, 98, count + 5, 5);
  Fill_Forest(117, 98, count + 10, 5);
  Fill_Forest(117, 135, count + 15, 5);
  count ++;
}

void Fill_Forest(uint8_t start, uint8_t stop, uint8_t Color_Begin, uint8_t grid) {
  uint8_t index = Color_Begin;
  if (start < stop) {
    for (uint8_t i = start; i < stop; i++) {
      leds[i] = ColorFromPalette( ForestColors_p, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
  else if (start > stop) {
    for (uint8_t i = start - 1; i >= stop; i--) {
      leds[i] = ColorFromPalette( ForestColors_p, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
}

//时钟背景——彩虹
void Rainbow2() {
  static uint8_t count;
  Fill_Rainbow2(0, 18, count, 4);
  Fill_Rainbow2(37, 18, count, 4);
  Fill_Rainbow2(37, 57, count, 4);
  Fill_Rainbow2(78, 57, count, 4);
  Fill_Rainbow2(78, 98, count + 4, 4);
  Fill_Rainbow2(117, 98, count + 8, 4);
  Fill_Rainbow2(117, 135, count + 12, 4);
  count ++;
}

void Fill_Rainbow2(uint8_t start, uint8_t stop, uint8_t Color_Begin, uint8_t grid) {
  uint8_t index = Color_Begin;
  if (start < stop) {
    for (uint8_t i = start; i < stop; i++) {
      leds[i] = ColorFromPalette( myColorPalette, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
  else if (start > stop) {
    for (uint8_t i = start - 1; i >= stop; i--) {
      leds[i] = ColorFromPalette( myColorPalette, index, BRIGHTNESS, LINEARBLEND);
      index += grid;
    }
  }
}

//随机显示时钟背景
void Random2() {
  static uint16_t count;
  static uint8_t index = random8(6);
  switch (index) {
    case 0: fill_solid(leds, NUM_LEDS, CHSV(0, 255, BRIGHTNESS)); break;
    case 1: Cloud(); break;
    case 2: Ocean(); break;
    case 3: Heat(); break;
    case 4: Forest(); break;
    case 5: Rainbow2(); break;
  }
  count++;
  if (count == 500) {
    index = random8(6);
    count = 0;
  }
}

//显示冒号
void DisplayDots(int pos) {
  if (39 + pos >= 37 && 39 + pos <= 56) {
    leds[39 + pos] = CRGB(R, G, B);
  }
  if (81 + pos >= 78 && 81 + pos <= 97) {
    leds[81 + pos] = CRGB(R, G, B);
  }
}

//显示破折号
void DisplayDash(int pos) {
  if (76 - pos >= 57 && 76 - pos <= 77) {
    leds[76 - pos] = CRGB(R, G, B);
  }
  if (75 - pos >= 57 && 75 - pos <= 77) {
    leds[75 - pos] = CRGB(R, G, B);
  }
}

//显示单个数字
void DisplayNumber(int pos, uint8_t num) {
  for (uint8_t i = 0; i < 13; i++) {
    uint8_t Value = Number[num][i];
    if (Value != 0) {
      if (Value <= 56 && Value >= 37) {
        if (Value + pos <= 56 && Value + pos >= 37 ) {
          leds[Value + pos] = CRGB(R, G, B);
        }
      }
      else if (Value <= 97 && Value >= 78) {
        if (Value + pos <= 97 && Value + pos >= 78) {
          leds[Value + pos] = CRGB(R, G, B);
        }
      }
      else if (Value <= 77 && Value >= 57) {
        if (Value - pos <= 77 && Value - pos >= 57) {
          leds[Value - pos] = CRGB(R, G, B);
        }
      }
      else if (Value <= 36 && Value >= 18) {
        if (Value - pos <= 36 && Value - pos >= 18) {
          leds[Value - pos] = CRGB(R, G, B);
        }
      }
      else if (Value <= 116 && Value >= 98) {
        if (Value - pos <= 116 && Value - pos >= 98) {
          leds[Value - pos] = CRGB(R, G, B);
        }
      }
    }
  }
}

//设置时间
void SetTime() {
  Time time = rtc.time();
  rtc.writeProtect(false);
  rtc.halt(false);
  Time t(time.yr, time.mon, time.date, Hour, Minute, 0, time.day);
  rtc.time(t);
  rtc.writeProtect(true);
  rtc.halt(true);
}

void TimeMode() {
  static boolean flag;
  static uint8_t count;
  static uint8_t cnt;
  switch (Mode3) {
    case 0: fill_solid(leds, NUM_LEDS, CHSV(0, 255, BRIGHTNESS)); break;
    case 1: Cloud(); break;
    case 2: Ocean(); break;
    case 3: Heat(); break;
    case 4: Forest(); break;
    case 5: Rainbow2(); break;
    case 6: Random2();break;
  }
  DisplayNumber(0, Hour / 10);
  DisplayNumber(4, Hour % 10);
  DisplayNumber(10, Minute / 10);
  DisplayNumber(14, Minute % 10);

  if (flag)DisplayDots(7);
  if (cnt == 50) {
    cnt = 0;
    flag = !flag;
  }
  if (count == 50) {
    count = 0;
    Time t = rtc.time();
    Hour = t.hr;
    Minute = t.min;
  }
  count ++;
  cnt++;
}

//滚动时间
void Scroll_Time() {
  static uint8_t pos;
  static uint8_t count;
  static uint8_t cnt;
  switch (Mode3) {
    case 0: fill_solid(leds, NUM_LEDS, CHSV(0, 255, BRIGHTNESS)); break;
    case 1: Cloud(); break;
    case 2: Ocean(); break;
    case 3: Heat(); break;
    case 4: Forest(); break;
    case 5: Rainbow2(); break;
    case 6: Random2();break;
  }

  //year
  DisplayNumber(16 - pos, Year / 1000);
  DisplayNumber(20 - pos, (Year % 1000) / 100);
  DisplayNumber(24 - pos, (Year % 100) / 10);
  DisplayNumber(28 - pos, Year % 10);
  DisplayDash(33 - pos);

  //month
  DisplayNumber(35 - pos, Month / 10);
  DisplayNumber(39 - pos, Month % 10);
  DisplayDash(44 - pos);

  //date
  DisplayNumber(46 - pos, Date / 10);
  DisplayNumber(50 - pos, Date % 10);

  //time
  DisplayNumber(57 - pos, Hour / 10);
  DisplayNumber(61 - pos, Hour % 10);
  DisplayDots(64 - pos);
  DisplayNumber(67 - pos, Minute / 10);
  DisplayNumber(71 - pos, Minute % 10);



  if (cnt == 7) {
    cnt = 0;
    pos++;
    if (pos == 75)pos = 0;
  }
  else if (count == 50) {
    count = 0;
    Time t = rtc.time();
    Year = t.yr;
    Month = t.mon;
    Date = t.date;
    Hour = t.hr;
    Minute = t.min;

  }
  count++;
  cnt++;
}

//互动模式
void UIMode() {
  Turn_OFF();
}

void alarm_irq() {
  isAlarm = true;
}

// 获取LED的位置
uint8_t Get_XY(uint8_t x, uint8_t y) {
  if (x >= kMatrixWidth || y >= kMatrixHeight) return LAST_VISIBLE_LED;
  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = XY_Table[i];
  return j;
}

//　设置固定颜色
void Solid_Color() {
  static uint8_t ColorIndex;
  for (uint8_t y = 0; y < kMatrixHeight; y++) {
    for (uint8_t x = 0, progress = 0; x < kMatrixWidth; x++, progress += 8)
      leds[Get_XY(x, y)] = ColorFromPalette(CloudColors_p, ColorIndex + progress, BRIGHTNESS, LINEARBLEND);
  }
  ColorIndex++;
}

//　模式1
void Cross_Snack() {
  static uint8_t ColorIndex;
  static uint8_t PositionIndex;
  leds[PositionIndex] = ColorFromPalette(ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
  leds[LAST_VISIBLE_LED - PositionIndex - 1] = ColorFromPalette(ColorPalette[Palette_Mode], ColorIndex + 64, BRIGHTNESS, LINEARBLEND);
  if (PositionIndex++ == LAST_VISIBLE_LED) {
    ColorIndex += 64;
    PositionIndex = 0;
    FastLED.clear();
  }
}

// 模式2
void Cross_Line() {
  static uint8_t ColorIndex;
  uint8_t BeatX = beatsin8(20, 0, kMatrixWidth - 1);
  uint8_t BeatY = beatsin8(20, 0, kMatrixHeight - 1);
  for (uint8_t i = 0; i < kMatrixWidth; i++)
    leds[Get_XY(i, BeatY)] = ColorFromPalette(ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
  for (uint8_t i = 0; i < kMatrixHeight; i++)
    leds[Get_XY(BeatX, i)] = ColorFromPalette(ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
  fadeToBlackBy(leds, LAST_VISIBLE_LED, BRIGHTNESS / 3);
  ColorIndex++;
}

// 模式3
void Cross_Dot() {
  static uint8_t ColorIndex;
  static boolean Direction;
  EVERY_N_MILLISECONDS(2000)
  Direction = !Direction;
  if (Direction) {
    uint8_t BeatA = beatsin8(30, 0, kMatrixWidth - 1);
    for (uint8_t y = 0; y < kMatrixHeight; y += 2) {
      leds[Get_XY(BeatA, y)] = ColorFromPalette(ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
      leds[Get_XY(kMatrixWidth - BeatA - 1, y + 1)] = ColorFromPalette(ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
    }
  } else {
    uint8_t BeatB = beatsin8(30, 0, kMatrixHeight - 1);
    for (uint8_t x = 0; x < kMatrixWidth; x += 2) {
      leds[Get_XY(x, BeatB)] = ColorFromPalette(ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
      leds[Get_XY(x + 1, kMatrixHeight - BeatB - 1)] = ColorFromPalette(ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
    }
  }
  fadeToBlackBy(leds, LAST_VISIBLE_LED, BRIGHTNESS / 3);
  ColorIndex++;
}

// 模式4
void Twinkle() {
  leds[random8(LAST_VISIBLE_LED)] = ColorFromPalette(ColorPalette[Palette_Mode], random8(255), BRIGHTNESS, LINEARBLEND);
  fadeToBlackBy(leds, LAST_VISIBLE_LED, BRIGHTNESS / 5);
}

// 渐变背景
void Faded_Background() {
  static int8_t ColorIndex;
  for (int8_t y = 0; y < kMatrixHeight; y++)
    for (int8_t x = 0, Progress = 0; x < kMatrixWidth; x++, Progress += 8)
      leds[Get_XY(x, y)] = ColorFromPalette(ColorPalette[Palette_Mode], ColorIndex + Progress, BRIGHTNESS, LINEARBLEND);
  int8_t beatA = beatsin8(30, -20, 20);
  int8_t beatB = beatsin8(20, -20, 20);
  ColorIndex += ((beatA + beatB) / 2);
}

// 显示数字
void Show_Num(uint8_t Position, uint8_t Number) {
  for (uint8_t i = 0; i < 13; i++) {
    uint8_t x = NUM[Number][i][0];
    uint8_t y = NUM[Number][i][1];
    if (x != 0 || y != 0) leds[Get_XY(x + Position, y)] = CRGB(R, G, B);
  }
}

// 显示时间
void Show_Time() {
  static boolean LED_Status;
  Solid_Color();
  Show_Num(0, HOUR / 10);
  Show_Num(4, HOUR % 10);
  Show_Num(10, MINUTE / 10);
  Show_Num(14, MINUTE % 10);
  if (LED_Status) Show_Num(7, 10);
  EVERY_N_MILLISECONDS(1000)
  LED_Status = !LED_Status;
  EVERY_N_MILLISECONDS(30000) {
    DateTime now = rtc.now();
    YEAR = now.year(), MONTH = now.month(), DATE = now.day(),
    HOUR = now.hour(), MINUTE = now.minute();
  }
}

// 显示日期
void Show_Date(uint8_t Speed = 150) {
  static uint8_t Scroll_Index;
  Solid_Color();
  EVERY_N_MILLISECONDS(30000) {
    DateTime now = rtc.now();
    YEAR = now.year(), MONTH = now.month(), DATE = now.day(),
    HOUR = now.hour(), MINUTE = now.minute();
  }
  EVERY_N_MILLISECONDS(Speed)
  Scroll_Index = (Scroll_Index + 1) % 75;

  // year
  Show_Num(16 - Scroll_Index, YEAR / 1000);
  Show_Num(20 - Scroll_Index, (YEAR % 1000) / 100);
  Show_Num(24 - Scroll_Index, (YEAR % 100) / 10);
  Show_Num(28 - Scroll_Index, YEAR % 10);
  Show_Num(32 - Scroll_Index, 11);

  // month
  Show_Num(35 - Scroll_Index, MONTH / 10);
  Show_Num(39 - Scroll_Index, MONTH % 10);
  Show_Num(43 - Scroll_Index, 11);

  // date
  Show_Num(46 - Scroll_Index, DATE / 10);
  Show_Num(50 - Scroll_Index, DATE % 10);

  // time
  Show_Num(57 - Scroll_Index, HOUR / 10);
  Show_Num(61 - Scroll_Index, HOUR % 10);
  Show_Num(64 - Scroll_Index, 10);
  Show_Num(67 - Scroll_Index, MINUTE / 10);
  Show_Num(71 - Scroll_Index, MINUTE % 10);
}

// 设置时间
void SetTime() {
  rtc.adjust(DateTime(YEAR, MONTH, DATE, HOUR, MINUTE, 0));
}

// publish message
void publishMessage() {
  String message = "{\"state\":\"off\",\"mode\":";
  if (isDisplay) message = "{\"state\":\"on\",\"mode\":";
  message = message + Mode + ",\"online\":\"on\",\"year\":" + YEAR + ",\"month\":" + MONTH + ",\"date\":" + DATE + ",\"hour\":" + HOUR + ",\"minute\":" + MINUTE + '}';
  mqtt.publish(mqtt_pub_topic, message, true, 0);
  websocket.textAll(message);
}

// 检查闹铃
void checkAlarm() {
  if (isAlarm) {
    if (rtc.alarmFired(1)) {
      isDisplay = false;
      rtc.clearAlarm(1);
    } else if (rtc.alarmFired(2)) {
      isDisplay = true;
      rtc.clearAlarm(2);
    }
    isAlarm = false;
  }
}

// 获取按钮操作
void checkButton() {
  if (!digitalRead(Button1)) {  // 上一个模式
    delay(10);
    if (!digitalRead(Button1)) {
      while (!digitalRead(Button1));
      if (Mode > 0) Mode--;
      else Mode = 7;
      publishMessage();
    }
  } else if (!digitalRead(Button2)) {  // 下一个模式
    delay(10);
    if (!digitalRead(Button1)) {
      while (!digitalRead(Button2));
      Mode = (Mode + 1) % 8;
      publishMessage();
    }
  } else if (!digitalRead(Button3)) {  // 更换背景
    delay(10);
    if (!digitalRead(Button1)) {
      while (!digitalRead(Button3));
      Palette_Mode = (Palette_Mode + 1) % 5;
      publishMessage();
    }
  } else if (!digitalRead(Button4)) {  // 打开/关闭显示
    delay(10);
    if (!digitalRead(Button1)) {
      while (!digitalRead(Button4));
      isDisplay = !isDisplay;
      publishMessage();
    }
  }
}

// 随机光效
void Random_Effects() {
  static uint8_t MODE = random(6);
  EVERY_N_MILLISECONDS(20000)
  MODE = random(6);
  switch (MODE) {
    case 0: Cross_Snack(); break;
    case 1: Cross_Line(); break;
    case 2: Cross_Dot(); break;
    case 3: Twinkle(); break;
    case 4: Faded_Background(); break;
  }
}

//　显示特效
void Display() {
  if (!isDisplay)FastLED.clear();
  else if (Mode == 0)Show_Time();
  else if (Mode == 1)Show_Date();
  else if (Mode == 2)Cross_Snack();
  else if (Mode == 3)Cross_Line();
  else if (Mode == 4)Cross_Dot();
  else if (Mode == 5)Twinkle();
  else if (Mode == 6)Faded_Background();
  else if (Mode = 7)Random_Effects();

  FastLED.show();
}

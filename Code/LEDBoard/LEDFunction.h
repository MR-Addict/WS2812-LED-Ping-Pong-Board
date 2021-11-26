uint8_t Get_XY (uint8_t x, uint8_t y) {
  if (x >= kMatrixWidth || y >= kMatrixHeight)
    return LAST_VISIBLE_LED;
  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = XY_Table[i];
  return j;
}

void Solid_Color() {
  static uint8_t ColorIndex;
  for (uint8_t y = 0; y < kMatrixHeight; y++) {
    for (uint8_t x = 0, progress = 0; x < kMatrixWidth; x++, progress += 8)
      leds[Get_XY(x, y)] = ColorFromPalette( ColorPalette[Palette_Mode], ColorIndex + progress, BRIGHTNESS, LINEARBLEND);
  }
  ColorIndex ++;
}

void Cross_Snack() {
  static uint8_t ColorIndex;
  static uint8_t PositionIndex;
  leds[PositionIndex] = ColorFromPalette( ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
  leds[LAST_VISIBLE_LED - PositionIndex - 1] = ColorFromPalette( ColorPalette[Palette_Mode], ColorIndex + 64, BRIGHTNESS, LINEARBLEND);
  if (PositionIndex++ == LAST_VISIBLE_LED) {
    ColorIndex += 64;
    PositionIndex = 0;
    FastLED.clear();
  }
}

void Cross_Line() {
  static uint8_t ColorIndex;
  uint8_t BeatX = beatsin8(20, 0, kMatrixWidth - 1);
  uint8_t BeatY = beatsin8(20, 0, kMatrixHeight - 1);
  for (uint8_t i = 0; i < kMatrixWidth; i++)leds[Get_XY(i, BeatY)] = ColorFromPalette( ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
  for (uint8_t i = 0; i < kMatrixHeight; i++)leds[Get_XY(BeatX, i)] = ColorFromPalette( ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
  fadeToBlackBy(leds, LAST_VISIBLE_LED, BRIGHTNESS / 3);
  ColorIndex++;
}

void Cross_Dot() {
  static uint8_t ColorIndex;
  static boolean Direction;
  EVERY_N_MILLISECONDS(2000)Direction = !Direction;
  if (Direction) {
    uint8_t BeatA = beatsin8(30, 0, kMatrixWidth - 1);
    for (uint8_t y = 0; y < kMatrixHeight; y += 2) {
      leds[Get_XY(BeatA, y)] = ColorFromPalette( ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
      leds[Get_XY(kMatrixWidth - BeatA - 1, y + 1)] = ColorFromPalette( ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
    }
  }
  else {
    uint8_t BeatB = beatsin8(30, 0, kMatrixHeight - 1);
    for (uint8_t x = 0; x < kMatrixWidth; x += 2) {
      leds[Get_XY(x, BeatB)] = ColorFromPalette( ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
      leds[Get_XY(x + 1, kMatrixHeight - BeatB - 1)] = ColorFromPalette( ColorPalette[Palette_Mode], ColorIndex, BRIGHTNESS, LINEARBLEND);
    }
  }
  fadeToBlackBy(leds, LAST_VISIBLE_LED, BRIGHTNESS / 3);
  ColorIndex++;
}

void Twinkle() {
  leds[random8(LAST_VISIBLE_LED)] = ColorFromPalette( ColorPalette[Palette_Mode], random8(255), BRIGHTNESS, LINEARBLEND);
  fadeToBlackBy( leds, LAST_VISIBLE_LED, BRIGHTNESS / 5);
}

void Faded_Background() {
  static int8_t ColorIndex;
  for (int8_t y = 0; y < kMatrixHeight; y++)
    for (int8_t x = 0, Progress = 0; x < kMatrixWidth; x++, Progress += 8)
      leds[Get_XY(x, y)] = ColorFromPalette( ColorPalette[Palette_Mode], ColorIndex + Progress, BRIGHTNESS, LINEARBLEND);
  int8_t beatA = beatsin8(30, -20, 20);
  int8_t beatB = beatsin8(20, -20, 20);
  ColorIndex += ((beatA + beatB) / 2);
}

void Show_Num(uint8_t Position , uint8_t Number) {
  for (uint8_t i = 0; i < 13; i++) {
    uint8_t x = NUM[Number][i][0];
    uint8_t y = NUM[Number][i][1];
    if (x != 0 || y != 0)leds[Get_XY(x + Position, y)] = CRGB(R, G, B);
  }
}

void Show_Char(uint8_t Position , uint8_t Number) {
  for (uint8_t i = 0; i < 30; i++) {
    uint8_t x = CHAR[Number][i][0];
    uint8_t y = CHAR[Number][i][1];
    if (x != 0 || y != 0)leds[Get_XY(x + Position, y)] = CRGB(R, G, B);
  }
}

void Show_Time() {
  static boolean LED_Status;
  Solid_Color();
  Show_Num(0, HOUR / 10);
  Show_Num(4, HOUR % 10);
  Show_Num(10, MINUTE / 10);
  Show_Num(14, MINUTE % 10);
  if (LED_Status)Show_Num(7, 10);
  EVERY_N_MILLISECONDS(1000)LED_Status = !LED_Status;
  EVERY_N_MILLISECONDS(30000) {
    DateTime now = rtc.now();
    YEAR = now.year(), MONTH = now.month(), DATE = now.day(), HOUR = now.hour(), MINUTE = now.minute();
  }
}

void Show_Date(uint8_t Speed = 150) {
  static uint8_t Scroll_Index;
  Solid_Color();
  EVERY_N_MILLISECONDS(30000) {
    DateTime now = rtc.now();
    YEAR = now.year(), MONTH = now.month(), DATE = now.day(), HOUR = now.hour(), MINUTE = now.minute();
  }
  EVERY_N_MILLISECONDS(Speed)Scroll_Index = (Scroll_Index + 1) % 75;

  //year
  Show_Num(16 - Scroll_Index, YEAR / 1000);
  Show_Num(20 - Scroll_Index, (YEAR % 1000) / 100);
  Show_Num(24 - Scroll_Index, (YEAR % 100) / 10);
  Show_Num(28 - Scroll_Index, YEAR % 10);
  Show_Num(32 - Scroll_Index, 11);

  //month
  Show_Num(35 - Scroll_Index, MONTH / 10);
  Show_Num(39 - Scroll_Index, MONTH % 10);
  Show_Num(43 - Scroll_Index, 11);

  //date
  Show_Num(46 - Scroll_Index, DATE / 10);
  Show_Num(50 - Scroll_Index, DATE % 10);

  //time
  Show_Num(57 - Scroll_Index, HOUR / 10);
  Show_Num(61 - Scroll_Index, HOUR % 10);
  Show_Num(64 - Scroll_Index, 10);
  Show_Num(67 - Scroll_Index, MINUTE / 10);
  Show_Num(71 - Scroll_Index, MINUTE % 10);
}

void Show_Character1() {
  Solid_Color();
  Show_Char(1, 0);
  Show_Char(9, 1);
}

void Show_Character2() {
  Solid_Color();
  Show_Char(2, 4);
  Show_Char(10, 5);
}

void Scroll_Character(uint8_t Speed = 150) {
  static uint8_t Scroll_Index;
  Solid_Color();
  Show_Char(16 - Scroll_Index, 0);
  Show_Char(24 - Scroll_Index, 1);
  Show_Char(32 - Scroll_Index, 2);
  Show_Char(39 - Scroll_Index, 3);
  EVERY_N_MILLISECONDS(Speed)Scroll_Index = (Scroll_Index + 1) % 46;
}

void Dynamic_Effects() {
  static uint8_t MODE = random(8);
  EVERY_N_MILLISECONDS(20000)MODE = random(8);
  switch (MODE) {
    case 0: Cross_Snack(); break;
    case 1: Cross_Line(); break;
    case 2: Cross_Dot(); break;
    case 3: Twinkle(); break;
    case 4: Solid_Color(); break;
    case 5: Faded_Background(); break;
    case 6: Show_Character1(); break;
    default: Scroll_Character(); break;
  }
}

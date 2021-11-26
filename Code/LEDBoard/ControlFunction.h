void Get_Button() {
  if (!digitalRead(Button1)) {
    if (Mode > 0)Mode--;
    else Mode = 2;
    Palette_Mode = 0;
    delay(10);
    while (!digitalRead(Button1));
  }
  else if (!digitalRead(Button2)) {
    Mode = (Mode + 1) % 3;
    Palette_Mode = 0;
    delay(10);
    while (!digitalRead(Button2));
  }
  else if (!digitalRead(Button3)) {
    Palette_Mode = (Palette_Mode + 1) % 5;
    delay(10);
    while (!digitalRead(Button3));
  }
  else if (!digitalRead(Button4)) {
    isDisplay = !isDisplay;
    delay(10);
    while (!digitalRead(Button4));
  }
}

void SetTime() {
  rtc.adjust(DateTime(YEAR, MONTH, DATE, HOUR, MINUTE, 0));
}

void Get_Serial() {
  if (SerialBT.available()) {
    uint8_t message = SerialBT.read();
    uint16_t parameter = SerialBT.parseInt();
    if (message == 'S')isDisplay = parameter;
    else if (message == 'A')Mode = parameter;
    else if (message == 'B')Palette_Mode = parameter;
    else if (message == 'R') {
      R = parameter;
      G = SerialBT.parseInt();
      B = SerialBT.parseInt();
    }
    else if (message == 'H') {
      HOUR = parameter;
      MINUTE = SerialBT.parseInt();
      SetTime();
    }
    else if (message == 'Y') {
      YEAR = parameter;
      MONTH = SerialBT.parseInt();
      DATE = SerialBT.parseInt();
      SetTime();
    }
  }
}

void Display() {
  if (isDisplay) {
    switch (Mode) {
      case 1: Show_Time(); break;
      case 2: Show_Date(); break;
      default: Dynamic_Effects(); break;
    }
  }
  else FastLED.clear();
  FastLED.show();
}

bool isDisplay = false;
volatile bool isAlarm = false;
uint8_t Mode = 0;
uint8_t R = 255, G = 11, B = 76;
uint8_t Palette_Mode = 0;
uint8_t MONTH, DATE, HOUR, MINUTE;
uint16_t YEAR;

const uint8_t LED_PIN = 32;
const uint8_t Button1 = 17;
const uint8_t Button2 = 18;
const uint8_t Button3 = 19;
const uint8_t Button4 = 23;
const uint8_t INT_PIN = 5;

const uint8_t BRIGHTNESS = 50;
const uint8_t kMatrixWidth = 21;
const uint8_t kMatrixHeight = 7;
const uint8_t LAST_VISIBLE_LED = 135;
const uint8_t NUM_LEDS = kMatrixWidth * kMatrixHeight;

const char* hostname = "ledboard";
const char* WIFI_SSID = "STAS-507";
const char* WIFI_PASSWORD = "manke666";

bool mqttBrokerFailed = false;
const char* mqtt_id = "school_office_clock";
const char* mqtt_broker = "172.18.224.1";
const char* mqtt_user = "client";
const char* mqtt_password = mqtt_user;
const char* mqtt_sub_topic = "school/office/clock/set";
const char* mqtt_pub_topic = "school/office/clock/status";
const char* mqtt_will_topic = mqtt_pub_topic;
const char* mqtt_will_msg = "{\"state\":\"off\",\"mode\":\"0\",\"online\":\"off\"}";

AsyncWebServer server(80);
AsyncWebSocket websocket("/ws");

WiFiClient espClient;
MQTTClient mqtt;

RTC_DS3231 rtc;
CRGB leds[NUM_LEDS];
CRGBPalette16 ColorPalette[5] = { RainbowColors_p, CloudColors_p, OceanColors_p, LavaColors_p, ForestColors_p };

const uint8_t XY_Table[] = {
  117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
  132, 133, 134, 144, 145, 146, 116, 115, 114, 113, 112, 111, 110, 109, 108,
  107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 143, 142, 78, 79, 80,
  81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
  96, 97, 141, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66,
  65, 64, 63, 62, 61, 60, 59, 58, 57, 140, 37, 38, 39, 40, 41,
  42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
  139, 138, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24,
  23, 22, 21, 20, 19, 18, 135, 136, 137, 0, 1, 2, 3, 4, 5,
  6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
};

uint8_t NUM[12][13][2] = {
  { { 2, 1 }, { 3, 1 }, { 4, 1 }, { 4, 2 }, { 4, 3 }, { 4, 4 }, { 4, 5 }, { 3, 5 }, { 2, 5 }, { 2, 4 }, { 2, 3 }, { 2, 2 } },            //数字0
  { { 2, 1 }, { 3, 1 }, { 3, 2 }, { 3, 3 }, { 3, 4 }, { 3, 5 }, { 2, 5 }, { 4, 5 } },                                                    //数字1
  { { 2, 1 }, { 3, 1 }, { 4, 1 }, { 4, 2 }, { 4, 3 }, { 3, 3 }, { 2, 3 }, { 2, 4 }, { 2, 5 }, { 3, 5 }, { 4, 5 } },                      //数字2
  { { 2, 1 }, { 3, 1 }, { 4, 1 }, { 4, 2 }, { 4, 3 }, { 3, 3 }, { 2, 3 }, { 4, 4 }, { 2, 5 }, { 3, 5 }, { 4, 5 } },                      //数字3
  { { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 3 }, { 4, 1 }, { 4, 2 }, { 4, 3 }, { 4, 4 }, { 4, 5 } },                                          //数字4
  { { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 3 }, { 4, 3 }, { 4, 4 }, { 4, 5 }, { 3, 5 }, { 2, 5 }, { 3, 1 }, { 4, 1 } },                      //数字5
  { { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 3 }, { 4, 3 }, { 4, 4 }, { 4, 5 }, { 3, 5 }, { 2, 5 }, { 3, 1 }, { 4, 1 }, { 2, 4 } },            //数字6
  { { 2, 1 }, { 3, 1 }, { 4, 1 }, { 4, 2 }, { 4, 3 }, { 4, 4 }, { 4, 5 } },                                                              //数字7
  { { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 3 }, { 4, 3 }, { 4, 4 }, { 4, 5 }, { 3, 5 }, { 2, 5 }, { 3, 1 }, { 4, 1 }, { 2, 4 }, { 4, 2 } },  //数字8
  { { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 3 }, { 4, 3 }, { 4, 4 }, { 4, 5 }, { 3, 5 }, { 3, 1 }, { 4, 1 }, { 4, 2 }, { 2, 5 } },            //数字9
  { { 3, 2 }, { 3, 4 } },                                                                                                                //冒号
  { { 2, 3 }, { 3, 3 } },                                                                                                                //破折号
};

//#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>

// Configure orientation of the display.
// 0 = none, 1 = 90 degrees clockwise, 2 = 180 degrees, 3 = 270 degrees CW
#define ROTATION 0
#define motor_pin 4
#define UNUSED_TIME 30

// Create OLED and INA219 globals.
Adafruit_SSD1306 display;
Adafruit_INA219 ina219;
// Keep track of total time and milliamp measurements for milliamp-hour computation.
uint32_t total_sec = 0;
float total_mA = 0.0;
unsigned long current_time;
unsigned long previous_time = 0;
unsigned long interval_time = 1000;

unsigned long previous_time_2 = 0;
unsigned long interval_time_2 = 1000;

static const unsigned char PROGMEM logo_bluethooth[] = { 0x30, 0xa8, 0x70, 0xa8, 0x30 };
static const unsigned char PROGMEM logo_serial[] = { 0x31, 0x80, 0x60, 0xc0, 0xff, 0xe0, 0x60, 0xc0, 0x31, 0x80 };
static const unsigned char PROGMEM logo_vibration[] = { 0x41, 0x00, 0x94, 0x80, 0xa2, 0x80, 0xaa, 0x80, 0xa2, 0x80, 0x94, 0x80, 0x41, 0x00 };

uint8_t ontime = 0;
uint8_t seconds = 0;
uint8_t minute = 0;
uint8_t hour = 0;

uint8_t count = 0;

bool battery_state;
bool Serial_com = LOW;
bool state;
bool off_flag = LOW;
void setup() {
  Serial.begin(115200);

  // while (!Serial) {
  // will pause Zero, Leonardo, etc until serial console opens
  // delay(1);
  //  }
  // Try to initialize the INA219

  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  // By default the INA219 will be calibrated with a range of 32V, 2A.
  // However uncomment one of the below to change the range.  A smaller
  // range can't measure as large of values but will measure with slightly
  // better precision.
  //ina219.setCalibration_32V_1A();
  //ina219.setCalibration_16V_400mA();

  // set pin 4 (vibration alert motor) as output
  pinMode(motor_pin, OUTPUT);
  pinMode(3, INPUT);

  // Setup the OLED display.
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Wire.setClock(400000);

  // Clear the display.
  display.clearDisplay();
  display.display();

  // Set rotation.
  display.setRotation(ROTATION);

  // Set text size and color.
  display.setTextColor(WHITE);

  for (int i = 0; i < 11; i++) {
    if (Serial == true) {
      Serial_com = HIGH;
      Serial.println("beginning Serial communication");
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(10, 0);
      display.print("AMP meter");
      display.drawFastHLine(0, 16, 128, WHITE);
      // display.drawFastHLine(0, , 128, WHITE);
      display.setTextSize(1);
      display.setCursor(0, 17);
      display.print("connected succesfullyvia usb!");
      display.display();
      delay(2000);
      break;
    } else {
      display.clearDisplay();

      display.setTextSize(2);
      display.setCursor(10, 0);
      display.print("AMP meter");
      display.drawFastHLine(0, 18, 128, WHITE);
      display.drawFastHLine(0, 17, 128, WHITE);

      display.setTextSize(1);
      display.setCursor(0, 20);
      display.print("Waiting for usb:");
      display.print(i);
      display.display();
    }
    delay(200);
  }

  display.clearDisplay();
  display.display();

  display.drawBitmap(96, 1, logo_bluethooth, 5, 5, WHITE);
  display.drawBitmap(114, 1, logo_serial, 11, 5, WHITE);
  display.drawBitmap(103, 0, logo_vibration, 9, 7,WHITE);
  display.drawFastHLine(0, 8, 128, WHITE);
  display.display();
  delay(2000);
}
void loop() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("now ins da loop");
  display.display();
  delay(2000);
}

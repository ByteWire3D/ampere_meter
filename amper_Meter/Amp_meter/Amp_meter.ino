// Feather Power Meter
//
// Small Feather-based power monitor using an INA219 breakout and
// monochrome OLED display.
//
// Author: Tony DiCola (modded by ladyada)
//
// Released under a MIT license: http://opensource.org/licenses/MIT
//#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>

// Configure orientation of the display.
// 0 = none, 1 = 90 degrees clockwise, 2 = 180 degrees, 3 = 270 degrees CW
#define ROTATION 0
#define LED_PIN 4

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
static const unsigned char PROGMEM logo_noserial[] = { 0x35, 0x80, 0x64, 0xc0, 0xf5, 0xe0, 0x64, 0xc0, 0x35, 0x80 };
static const unsigned char PROGMEM logo_vibration[] = { 0x41, 0x00, 0x94, 0x80, 0xa2, 0x80, 0xaa, 0x80, 0xa2, 0x80, 0x94, 0x80, 0x41, 0x00 };
static const unsigned char PROGMEM logo_main_small[] = { 0xf5, 0xe0, 0xa4, 0xa0, 0xf5, 0xe0 };
static const unsigned char PROGMEM logo_main_big[] = { 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00,
                                                       0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00,
                                                       0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00,
                                                       0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00,
                                                       0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff,
                                                       0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff };
static const unsigned char PROGMEM square_8X8[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
/*
display.drawBitmap(96, 1, logo_bluethooth, 5, 5, WHITE);
display.drawBitmap(114, 1, logo_serial, 11, 5,WHITE);
display.drawBitmap(103, 0, logo_vibration, 9, 7, WHITE);
*/
uint8_t ontime = 0;
uint8_t seconds = 0;
uint8_t minute = 0;
uint8_t hour = 0;

long analoglow = 10;
long analoghigh = 240;
long volt_low = 2.2;
long volt_high = 3.0;


void setup() {
  Serial.begin(115200);

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

  // set pin 4 as output for the ledss
  pinMode(LED_PIN, OUTPUT);

  // Setup the OLED display.
  setup_display();
}

void loop() {
  powerupdate();
}

void powerupdate() {
  current_time = millis();
  if (current_time - previous_time >= interval_time) {
    update_power_display();
    previous_time = current_time;
  }
}

void update_power_display() {
  // Read voltage and current from INA219.
  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();

  // Compute load voltage, power, and milliamp-hours.
  float loadvoltage = busvoltage + (shuntvoltage / 1000);
  float power_mW = loadvoltage * current_mA;
  (void)power_mW;

  total_mA += current_mA;
  total_sec += 1;
  float total_mAH = total_mA / 3600.0;
  (void)total_mAH;
  // Update display.
  update_display(loadvoltage, current_mA, power_mW, total_mAH);
  //set light brightness to 20%
  convert_percentage(7.5);
}

void printSIValue(float value, const char* units, int precision, int maxWidth) {
  // Print a value in SI units with the units left justified and value right justified.
  // Will switch to milli prefix if value is below 1.

  // Add milli prefix if low value.
  if (fabs(value) < 1.0) {
    display.print('m');
    maxWidth -= 1;
    value *= 1000.0;
    precision = max(0, precision - 3);
  }

  // Print units.
  display.print(units);
  maxWidth -= strlen(units);

  // Leave room for negative sign if value is negative.
  if (value < 0.0) {
    maxWidth -= 1;
  }

  // Find how many digits are in value.
  int digits = ceil(log10(fabs(value)));
  if (fabs(value) < 1.0) {
    digits = 1;  // Leave room for 0 when value is below 0.
  }

  // Handle if not enough width to display value, just print dashes.
  if (digits > maxWidth) {
    // Fill width with dashes (and add extra dash for negative values);
    for (int i = 0; i < maxWidth; ++i) {
      display.print('-');
    }
    if (value < 0.0) {
      display.print('-');
    }
    return;
  }

  // Compute actual precision for printed value based on space left after
  // printing digits and decimal point.  Clamp within 0 to desired precision.
  int actualPrecision = constrain(maxWidth - digits - 1, 0, precision);

  // Compute how much padding to add to right justify.
  int padding = maxWidth - digits - 1 - actualPrecision;
  for (int i = 0; i < padding; ++i) {
    display.print(' ');
  }

  // Finally, print the value!
  display.print(value, actualPrecision);
}

void on_time() {
  seconds = ontime;
  if (seconds >= 60) {
    minute += 1;
    ontime = 0;
    seconds = 0;
  }
  if (minute >= 60) {
    hour += 1;
    minute = 0;
  }
  display.print("0");
  display.print(hour);
  display.print(":");
  if (minute < 10) {
    display.print("0");
  }
  display.print(minute);
  display.print(":");
  if (seconds < 10) {
    display.print("0");
  }
  display.print(seconds);
  display.drawFastHLine(0, 8, 128, WHITE);
  ontime += 1;
}

void setup_display() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Wire.setClock(400000);

  // Clear the display.
  display.clearDisplay();
  display.display();

  // Set rotation.
  display.setRotation(ROTATION);

  // Set text size and color.
  display.setTextColor(WHITE);

  boot_up();
  for (int i = 0; i < 11; i++) {
    if (Serial == true) {
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
      display.drawFastHLine(0, 16, 128, WHITE);

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
}

void update_display(float loadvoltage, float current_mA, float power_mW, float total_mAH) {
  screen_1(loadvoltage, current_mA, power_mW, total_mAH);

  if (Serial == true) {
    //Serial.println("serial == true");
    display.drawBitmap(114, 1, logo_serial, 11, 5, WHITE);

    Serial.print("Voltage: ");
    Serial.print(loadvoltage);
    Serial.print(" | ");
    Serial.print("Ampere: ");
    Serial.print(current_mA);
    Serial.print(" | ");
    Serial.print("Watt: ");
    Serial.print(power_mW);
    Serial.print(" | ");
    Serial.print("AH: ");
    Serial.print(total_mAH);
    Serial.println("");
  } else {
    //Serial.println("serial == false");
    display.drawBitmap(114, 1, logo_noserial, 11, 5, WHITE);
  }
  if (Serial == true) {
  }
  display.display();
}

void screen_1(float loadvoltage, float current_mA, float power_mW, float total_mAH) {
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.clearDisplay();
  on_time();
  display.setCursor(0, 16);
  printSIValue(loadvoltage, "V:", 2, 10);
  // Mode 0, display volts and amps.

  display.setCursor(0, 24);
  printSIValue(current_mA / 1000.0, "A:", 5, 10);

  display.setCursor(64, 16);
  printSIValue(power_mW, "W:", 2, 10);

  display.setCursor(64, 24);
  printSIValue(total_mAH, "AH:", 5, 10);
}

void boot_up() {
  int logo_delaytime = 50;
  convert_percentage(5);

  display.drawBitmap(86, 21, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(95, 3, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(104, 3, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(60, 12, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(104, 12, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(34, 21, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(60, 3, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(60, 21, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(77, 21, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(95, 21, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(43, 3, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(86, 12, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(34, 3, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(16, 12, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(34, 12, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(43, 21, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(104, 21, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(25, 3, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(16, 3, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(16, 21, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(25, 21, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(77, 3, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();

  display.drawBitmap(86, 3, square_8X8, 8, 8, WHITE);
  delay(logo_delaytime);
  display.display();
  delay(500);

  //display.clearDisplay();
  //display.display();
  //delay(10);

  //display.drawBitmap(20, 4, logo_main_big, 88, 24, WHITE);
  //convert_percentage(45);
  //display.display();
 
  delay(1000);
}
void convert_percentage(long percentage) {
  percentage = map(percentage, 0, 100, analoglow, analoghigh);
 // Serial.println(percentage);
  analogWrite(4, percentage);
}
void convert_volt(long des_voltage) {
  des_voltage = map(des_voltage, volt_low, volt_high, analoglow, analoghigh);
  //Serial.println(des_voltage);
  analogWrite(4, des_voltage);
}


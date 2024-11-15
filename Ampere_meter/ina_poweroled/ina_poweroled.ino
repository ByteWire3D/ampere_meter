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
#define ROTATION     0

// Create NeoPixel, OLED and INA219 globals.
Adafruit_SSD1306 display;
Adafruit_INA219 ina219;
// Keep track of total time and milliamp measurements for milliamp-hour computation.
uint32_t total_sec = 0;
float total_mA = 0.0;
unsigned long current_time;
unsigned long previous_time = 0;
unsigned long interval_time = 500;

uint8_t counter = 0;
void powerupdate() {
  current_time = millis();
  if (current_time - previous_time >= interval_time) {
    update_power_display();
  }

void setup()   {
  Serial.begin(115200);
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }
  // Try to initialize the INA219
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  // By default the INA219 will be calibrated with a range of 32V, 2A.
  // However uncomment one of the below to change the range.  A smaller
  // range can't measure as large of values but will measure with slightly
  // better precision.
  //ina219.setCalibration_32V_1A();
  //ina219.setCalibration_16V_400mA();

  // Setup the OLED display.
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Wire.setClock(400000);

  // Clear the display.
  display.clearDisplay();
  display.display();

  // Set rotation.
  display.setRotation(ROTATION);

  // Set text size and color.
  display.setTextSize(2);
  display.setTextColor(WHITE);
}

void loop() {
 powerupdate();
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
  display.clearDisplay();
  display.setCursor(0,0);

  // Mode 0, display volts and amps.
  printSIValue(loadvoltage, "V:", 2, 10);
  display.setCursor(0, 16);
  printSIValue(current_mA/1000.0, "A:", 5, 10);

  display.display();
}

void printSIValue(float value, const char* units, int precision, int maxWidth) {
  // Print a value in SI units with the units left justified and value right justified.
  // Will switch to milli prefix if value is below 1.

  // Add milli prefix if low value.
  if (fabs(value) < 1.0) {
    display.print('m');
    maxWidth -= 1;
    value *= 1000.0;
    precision = max(0, precision-3);
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
    digits = 1; // Leave room for 0 when value is below 0.
  }

  // Handle if not enough width to display value, just print dashes.
  if (digits > maxWidth) {
    // Fill width with dashes (and add extra dash for negative values);
    for (int i=0; i < maxWidth; ++i) {
      display.print('-');
    }
    if (value < 0.0) {
      display.print('-');
    }
    return;
  }

  // Compute actual precision for printed value based on space left after
  // printing digits and decimal point.  Clamp within 0 to desired precision.
  int actualPrecision = constrain(maxWidth-digits-1, 0, precision);

  // Compute how much padding to add to right justify.
  int padding = maxWidth-digits-1-actualPrecision;
  for (int i=0; i < padding; ++i) {
    display.print(' ');
  }

  // Finally, print the value!
  display.print(value, actualPrecision);
}



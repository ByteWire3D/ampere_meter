long analoglow = 10;
long analoghigh = 240;
long volt_low = 2.2;
long volt_high = 3.0;

void setup() {
  Serial.begin(115200);
  pinMode(4, OUTPUT);
}

void loop() {
  /*
  convert_volt(2.2);
  delay(1000);
  convert_volt(3);
  delay(1000);
  */
  for (int i = 0; i < 101; i++) {
    convert_percentage(i);
    delay(50);
    if (i == 100) {
      continue;
    }
  }
  for (int i = 100; i > 0; i--) {
    convert_percentage(i);
    delay(50);
    if (i == 0) {
      continue;
    }
  }
}
void convert_percentage(long percentage) {
  percentage = map(percentage, 0, 100, analoglow, analoghigh);
  Serial.println(percentage);
  analogWrite(4, percentage);
}
void convert_volt(long des_voltage) {
  des_voltage = map(des_voltage, volt_low, volt_high, analoglow, analoghigh);
  Serial.println(des_voltage);
  analogWrite(4, des_voltage);
}

#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// === HC-SR04 Pins ===
const int trigPin = 10;
const int echoPin = 11;
const int switchPin = 7; // Toggle switch for MPU/Camera mode

// === Servo and LCD ===
Servo servo;
const int servoPin = 9;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Use 0x3F if needed
Adafruit_MPU6050 mpu;

// === Variables ===
int angle = 90;
String inputData = "";
bool lastSwitchState = HIGH;
bool mpuMode = true;
unsigned long lastUpdate = 0;
unsigned long lastModeDisplayTime = 0;
bool showModeName = true;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.begin();

  servo.attach(servoPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(switchPin, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Aim Assist ON ");
  delay(1500);
  lcd.clear();
}

float getMPUAngle() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float pitch = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  return constrain(pitch + 90, 0, 180);
}

void loop() {
  bool currentSwitchState = digitalRead(switchPin);
  if (currentSwitchState != lastSwitchState) {
    delay(50); // debounce
    currentSwitchState = digitalRead(switchPin);
    if (currentSwitchState != lastSwitchState) {
      mpuMode = currentSwitchState == HIGH;
      showModeName = true;
      lastModeDisplayTime = millis();
      lastSwitchState = currentSwitchState;
    }
  }

  if (showModeName) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(mpuMode ? "Mode: MPU" : "Mode: CAM");
    showModeName = false;
  }

  if (millis() - lastModeDisplayTime < 2000) {
    return;
  }

  if (mpuMode) {
    if (millis() - lastUpdate > 200) {
      angle = getMPUAngle();
      servo.write(angle);
      lastUpdate = millis();

      long duration;
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH, 30000);
      int distance = duration * 0.034 / 2;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MPU Angle: ");
      lcd.print(angle);
      lcd.print((char)223);
      lcd.setCursor(0, 1);
      lcd.print("Dist: ");
      lcd.print(distance);
      lcd.print(" cm");
    }
  } else {
    if (Serial.available()) {
      char incomingChar = Serial.read();

      if (incomingChar == '\n') {
        angle = inputData.toInt();
        angle = constrain(angle, 0, 180);
        servo.write(angle);

        long duration;
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH, 30000);
        int distance = duration * 0.034 / 2;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Cam Angle: ");
        lcd.print(angle);
        lcd.print((char)223);
        lcd.setCursor(0, 1);
        lcd.print("Dist: ");
        lcd.print(distance);
        lcd.print(" cm");

        inputData = "";
      } else {
        inputData += incomingChar;
      }
    }
  }
}

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// HC-SR04
#define TRIG_PIN 9
#define ECHO_PIN 10

// LEDs
#define GREEN_LED 2
#define YELLOW_LED 3
#define RED_LED 4

// Buzzer
#define BUZZER 5

// Height of HC-SR04 from river bed
const float SENSOR_HEIGHT = 14.0;

// Water-level thresholds
const float NORMAL_LEVEL = 3.0;
const float WARNING_LEVEL = 6.0;
const float DANGER_LEVEL = 8.0;


void setup() {

  // Sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // LED pins
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // Buzzer
  pinMode(BUZZER, OUTPUT);

  // Serial Monitor
  Serial.begin(9600);

  // LCD
  lcd.init();
  lcd.backlight();

  // Startup screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FLOOD MONITOR");

  lcd.setCursor(0, 1);
  lcd.print("SYSTEM READY");

  delay(2000);

  lcd.clear();
}


void loop() {

  // ------------------------------------------------
  // 1. SEND ULTRASONIC PULSE
  // ------------------------------------------------

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);


  // ------------------------------------------------
  // 2. READ ECHO
  // ------------------------------------------------

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);


  // If sensor doesn't receive an echo
  if (duration == 0) {

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("SENSOR ERROR");

    lcd.setCursor(0, 1);
    lcd.print("CHECK HC-SR04");

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    delay(500);

    return;
  }


  // ------------------------------------------------
  // 3. CALCULATE DISTANCE
  // ------------------------------------------------

  float distance = duration * 0.0343 / 2;


  // ------------------------------------------------
  // 4. CALCULATE WATER LEVEL
  // ------------------------------------------------

  float waterLevel = SENSOR_HEIGHT - distance;


  // Prevent negative readings
  if (waterLevel < 0) {
    waterLevel = 0;
  }


  // ------------------------------------------------
  // 5. DISPLAY WATER LEVEL
  // ------------------------------------------------

  lcd.setCursor(0, 0);
  lcd.print("LEVEL: ");
  lcd.print(waterLevel, 1);
  lcd.print(" cm   ");


  // ------------------------------------------------
  // 6. NORMAL LEVEL
  // LESS THAN 3 CM
  // ------------------------------------------------

  if (waterLevel < NORMAL_LEVEL) {

    // Green ON
    digitalWrite(GREEN_LED, HIGH);

    // Yellow OFF
    digitalWrite(YELLOW_LED, LOW);

    // Red OFF
    digitalWrite(RED_LED, LOW);

    // Buzzer OFF
    digitalWrite(BUZZER, LOW);

    lcd.setCursor(0, 1);
    lcd.print("STATUS: NORMAL  ");
  }


  // ------------------------------------------------
  // 7. WARNING LEVEL
  // 3 CM TO LESS THAN 6 CM
  // ------------------------------------------------

  else if (waterLevel < WARNING_LEVEL) {

    // Green OFF
    digitalWrite(GREEN_LED, LOW);

    // Yellow ON
    digitalWrite(YELLOW_LED, HIGH);

    // Red OFF
    digitalWrite(RED_LED, LOW);

    // Buzzer OFF
    digitalWrite(BUZZER, LOW);

    lcd.setCursor(0, 1);
    lcd.print("STATUS: WARNING ");
  }


  // ------------------------------------------------
  // 8. DANGER LEVEL
  // 6 CM AND ABOVE
  // ------------------------------------------------

  else {

    // Green OFF
    digitalWrite(GREEN_LED, LOW);

    // Yellow OFF
    digitalWrite(YELLOW_LED, LOW);

    // Red ON
    digitalWrite(RED_LED, HIGH);

    // Buzzer ON
    digitalWrite(BUZZER, HIGH);

    lcd.setCursor(0, 1);

    if (waterLevel >= DANGER_LEVEL) {
      lcd.print("MAX LEVEL!      ");
    }
    else {
      lcd.print("!! DANGER !!    ");
    }
  }


  // ------------------------------------------------
  // 9. SERIAL MONITOR
  // ------------------------------------------------

  Serial.print("Distance from sensor: ");
  Serial.print(distance);
  Serial.print(" cm");

  Serial.print(" | Water Level: ");
  Serial.print(waterLevel);
  Serial.println(" cm");


  delay(500);
}

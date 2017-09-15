/*
  Liquid Level Sensor
  Copyright (c) 2017 Subhajit Das

  Licence Disclaimer:

   This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// include the library code:
#include <LiquidCrystal.h>
#include <NewPing.h>
#include <EEPROM.h>

//defining motor output pin
#define MOTOR_PIN 9

// defining motor max level and min level reading pins
#define MOTOR_CAL_MAX_PIN A0
#define MOTOR_CAL_MIN_PIN A1

// defining calibration pin for zero level cal
#define LEVEL_CAL_PIN A3

// pin to start motor Max and Min level
#define MOTOR_TRIGGER_CAL_PIN A2

//EEPROM location map for data storage
#define GROUND_DIST_ADDR 0
#define MAX_LEVEL_ADDR 1
#define MIN_LEVEL_ADDR 2

// pins for sonar object
#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     12  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 250 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

/* LCD circuit:
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   LCD VDD pin to Diode to GND
*/
// for v1.x design
#define RS 7
#define E 6
#define DB0 10
#define DB1 11
#define DB2 A5
#define DB3 A4
#define DB4 5
#define DB5 2
#define DB6 4
#define DB7 3
/*
  // for v2.x design
  #define RS 10
  #define E 11
  #define DB0 8
  #define DB1 7
  #define DB2 6
  #define DB3 5
  #define DB4 4
  #define DB5 3
  #define DB6 A3
  #define DB7 A4
*/
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(RS, E, DB0, DB1, DB2, DB3, DB4, DB5, DB6, DB7);

// storage for actual distance to zer level from sonar
unsigned int GroundDistance;

// temporary min and max level for motor triggering
byte MaxLevel;
byte MinLevel;

void setup() {
  Serial.begin(9600); // Open serial monitor at 9600 baud to see ping results.

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();

  // Print initialization message to the LCD.
  lcd.print("Initializing....");

  // aquiring preset Max and Min levels of previous motor calibration
  MaxLevel = EEPROM.read(MAX_LEVEL_ADDR);
  MinLevel = EEPROM.read(MIN_LEVEL_ADDR);

  // initiating output for Motor/Pump
  pinMode(MOTOR_PIN, OUTPUT);

  delay(200);
}

void loop() {
  GroundDistance = EEPROM.read(GROUND_DIST_ADDR); // Ground Distance is stored in location 0

  unsigned int Distance, uS;
  byte LiquidHeight;

  // Send ping, get ping time in microseconds (uS)
  uS = sonar.ping_median(20);

  // Calculate distance and height
  Distance = uS / US_ROUNDTRIP_CM; // "US_ROUNDTRIP_CM" is a constant defined in NewPing
  // Distance has to be less than GroundDistance
  if (Distance < GroundDistance)
    LiquidHeight = GroundDistance - Distance;
  else
    LiquidHeight = 0;  // in case of out of range level values

  // printing ping distance (actual distance) values to Serial
  Serial.print("Ping: ");
  Serial.print(Distance); // Convert ping time to distance in cm
  Serial.println(" cm");

  // Output data into Serial
  Serial.print("Height: ");
  Serial.print(LiquidHeight);
  Serial.println(" cm");

  // Output data into LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Height:");
  lcd.setCursor(0, 1);
  lcd.print(LiquidHeight);
  lcd.print(" cm");

  if (LiquidHeight < MinLevel)
    digitalWrite(MOTOR_PIN, HIGH);
  else if (LiquidHeight > MaxLevel)
    digitalWrite(MOTOR_PIN, LOW);

  // settings are calibrated for Motor Level
  if (digitalRead(MOTOR_TRIGGER_CAL_PIN)) {
    setMotorTriggerLevel();
  }

  // settings are calibrated for GroundDistance
  if (digitalRead(LEVEL_CAL_PIN)) {
    calibrate();
  }

  delay(500);
}

// system is calibrated to current distance
void calibrate() {
  // setting LCD
  lcd.clear();
  lcd.print("Calibrating...");
  delay(200);

  unsigned int Distance = 0;
  unsigned int uS;

  // measuring distance
  uS = sonar.ping_median(50); // Send ping, get ping time in microseconds (uS).
  Distance = uS / US_ROUNDTRIP_CM;
  EEPROM.write(GROUND_DIST_ADDR, (byte) Distance);

  // finishing calibration
  lcd.clear();
  lcd.print("Restarting...");
  delay(200);
}

void setMotorTriggerLevel() {
  while (true) {
    // setting motor trigger levels
    // setting MAX LEVEL
    MaxLevel = map(analogRead(MOTOR_CAL_MAX_PIN), 0, 1023, 0, MAX_DISTANCE);
    EEPROM.write(MAX_LEVEL_ADDR, (byte) MaxLevel);

    // setting MIN LEVEL
    byte tempMinLevel = map(analogRead(MOTOR_CAL_MIN_PIN), 0, 1023, 0, MAX_DISTANCE);
    // as MIN LEVEL can not be greater than MAX LEVEL
    if (tempMinLevel < MaxLevel)
      MinLevel = tempMinLevel;  // as received MIN LEVEL is acceptable
    else
      MinLevel = MaxLevel;  //  as received MIN LEVEL is invalid
    EEPROM.write(MIN_LEVEL_ADDR, (byte) MinLevel);

    // showing the current set Max and Min Levels to Serial Monitor
    Serial.print("Max Level : ");
    Serial.println(MaxLevel);
    Serial.print("Min Level : ");
    Serial.println(MinLevel);

    // showing the current set Max and Min Levels to LCD
    lcd.clear();
    lcd.print("Max Level : ");
    lcd.print(MaxLevel);
    lcd.setCursor(0, 1);
    lcd.print("Min Level : ");
    lcd.print(MinLevel);

    // stops calibrating if the trigger button is opened
    if (digitalRead(MOTOR_TRIGGER_CAL_PIN) == LOW) {
      break;
    }

    delay(500);
  }
}

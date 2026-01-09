/*
 * Author: Dean Etim (Radiant Tech Nig. Ltd.)
 * Last Updated: 01/08/2025
 * Version: Bluetooth-Controlled Smart Bin
 */

#include <ESP32Servo.h>
#include "BluetoothSerial.h"

// Objects
BluetoothSerial SerialBT;
Servo lidServo;

// Pin definitions
const int SERVO_PIN = 15;
const int TRIG_PIN  = 14;
const int ECHO_PIN  = 12;

// Distance measurement
const float conversionFactor = 0.0171;
const unsigned long interval = 300;
unsigned long lastDistanceTime = 0;
int distance = 0;

// State
bool lidOpen = false;

// Function prototypes
void openLid();
void closeLid();
int measureDistance();

void setup() {
  Serial.begin(115200);
  SerialBT.begin("SmartBin_BT");   // Bluetooth device name

  lidServo.attach(SERVO_PIN);
  lidServo.write(180);             // Closed position

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Bluetooth Smart Bin Ready. Waiting for commands...");
}

void loop() {
  // Handle Bluetooth command input
  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');
    command.trim();

    if (!lidOpen && command.equalsIgnoreCase("open")) {
      openLid();
      lidOpen = true;
      SerialBT.println("Lid opened");
    }
    else if (lidOpen && command.equalsIgnoreCase("open")) {
      SerialBT.println("Lid is opened already!");
    }
    else if (lidOpen && command.equalsIgnoreCase("close")) {
      closeLid();
      lidOpen = false;
      SerialBT.println("Lid closed");
    }
    else if (!lidOpen && command.equalsIgnoreCase("close")) {
      SerialBT.println("Lid is closed already!");
    }
    else {
      SerialBT.println("Unknown command. Use 'open' or 'close'.");
    }
  }
}

int measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  return duration * conversionFactor;
}

void openLid() {
  for (int angle = 180; angle >= 95; angle--) {
    lidServo.write(angle);
    delay(20);
  }
}

void closeLid() {
  for (int angle = 95; angle <= 180; angle++) {
    lidServo.write(angle);
    delay(20);
  }
}

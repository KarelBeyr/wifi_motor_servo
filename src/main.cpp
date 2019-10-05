#include <Arduino.h>
#include "debugUtils.h"
#include "wifimodule.h"
#include "esp_system.h"
#include "watchdog.h"
#include <HTTPClient.h>
#include <ESP32Servo.h>

Servo myServo;

const byte pinNumber = 2;
const byte pin2Number = 16;
const byte ledChannel0 = 2; //?
const byte ledChannel1 = 3; //?
const byte servoPin = 4;
const int freq = 5000;  //PWM freq
const int resolution = 10; //Resolution 8, 10, 12, 15 (asi je to 2^resolution, tzn 10~1024 kroku)

void setupRelay()
{
  pinMode(pinNumber, OUTPUT);
  digitalWrite(pinNumber, LOW);
  pinMode(pin2Number, OUTPUT);
  digitalWrite(pin2Number, LOW);
  pinMode(servoPin, OUTPUT);
}

void setup()
{
  setupWatchdog(90);
  setupRelay();
  DEBUG_SERIAL_START(115200);
  setupWifiServer();
  ledcSetup(ledChannel0, freq, resolution);
  ledcSetup(ledChannel1, freq, resolution);
  ledcAttachPin(pinNumber, ledChannel0);
  ledcAttachPin(pin2Number, ledChannel1);
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 1000, 2000);
  myServo.write(160);
}

void loop()
{
  feedWatchdog();
  maybeServeClient(ledChannel0, ledChannel1, myServo);
}

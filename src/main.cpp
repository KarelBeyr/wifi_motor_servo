#include <Arduino.h>
#include "debugUtils.h"
#include "wifimodule.h"
#include "esp_system.h"
#include "watchdog.h"
#include <HTTPClient.h>

const byte pinNumber = 2;
const byte pin2Number = 4;
const byte ledChannel0 = 0; //?
const byte ledChannel1 = 1; //?
const int freq = 5000;  //PWM freq
const int resolution = 10; //Resolution 8, 10, 12, 15 (asi je to 2^resolution, tzn 10~1024 kroku)

void setupRelay()
{
  pinMode(pinNumber, OUTPUT);
  digitalWrite(pinNumber, LOW);
  pinMode(pin2Number, OUTPUT);
  digitalWrite(pin2Number, LOW);
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
}

void loop()
{
  feedWatchdog();
  maybeServeClient(ledChannel0, ledChannel1);
}

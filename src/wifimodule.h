#ifndef Wifimodule_h
#define Wifimodule_h
#include <ESP32Servo.h>

void setupWifiServer();
void maybeServeClient(byte ch0, byte ch1, Servo myServo);
#endif
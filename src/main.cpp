#include <Arduino.h>
#include "MySD.hpp"
#include <SPI.h>

#define SD_CS_PIN 9 // Connected to HSPI (SPI 2) Refer to ESP32-S3 documentation for wiring
// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}
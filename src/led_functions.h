#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>  // ensures types like uint8_t and delay() work

// Defining ESP32 LEDs
#define RGB_PIN 47
#define NUM_LEDS 1

extern Adafruit_NeoPixel rgb_led;  // declare the LED object

// Functions
void initLED();
void setColor(uint8_t r, uint8_t g, uint8_t b);
void flashColor(uint8_t r, uint8_t g, uint8_t b, int times, int onTime = 100, int offTime = 100);

#endif